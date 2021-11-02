#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <sstream>

#include <boinc/boinc_api.h>

#include "calculate.h"

class Redirect {
public:
    Redirect() {
        original_cerr = std::cerr.rdbuf(std::cout.rdbuf());
        original_cout = std::cout.rdbuf(redirected.rdbuf());
    }
    ~Redirect() {
        std::cout.rdbuf(original_cout);
        std::cerr.rdbuf(original_cerr);
    }

    [[nodiscard]] std::string getline() {
        const auto data = redirected.str();
        if (data.length() == position) {
            return {};
        }
        const auto eol = data.find_first_of('\n', position);
        std::string::size_type count = eol - position;
        if (eol == std::string::npos) {
            count = data.length() - position;
        }
        std::string result;
        result.resize(count);
        data.copy(result.data(), count, position);
        if (eol != std::string::npos) {
            position = eol + 1;
        }
        size = eol == std::string::npos ? data.length() : position;
        return result;
    }

    [[nodiscard]] bool hasNewData() const {
        return redirected.str().length() != size;
    }

private:
    std::stringstream redirected {};
    std::streambuf* original_cerr { nullptr };
    std::streambuf* original_cout { nullptr };
    std::string::size_type position = 0;
    std::string::size_type size = 0;
};

inline void help() {
    std::cerr << "Usage:" << std::endl;
    std::cerr << "boinc-autodock-vina config.json" << std::endl;
}

inline void readCout(Redirect& redirect) {
    while (redirect.hasNewData())
    {
        const auto& data = redirect.getline();
        std::cerr << data;
        if (!data.empty() && data[0] == '*') {
            boinc_fraction_done((static_cast<double>(data.length() - 1) * 2.) / 100);
            std::cerr << "\r";
        }
        else {
            std::cerr << std::endl;
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        help();
        return 1;
    }

    char buf[256];

    try {
        if (const auto res = boinc_init()) {
            std::cerr << boinc_msg_prefix(buf, sizeof(buf)) << " boinc_init failed with error code " << res << std::endl;
            return res;
        }

        std::atomic done(false);
        std::atomic result(false);

        std::string json(argv[1]);

        Redirect redirect;

        std::thread worker([&done, &result, &json] {
            try {
                result = calculate(json);
            }
            catch (const std::exception& ex)
            {
                char str[256];
                std::cerr << boinc_msg_prefix(str, sizeof(str)) << " docking failed: " << ex.what() << std::endl;
                result = false;
            }

            done = true;
            });

        while (!done) {
            readCout(redirect);
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1s);
        }
        readCout(redirect);

        if (!result) {
            std::cerr << boinc_msg_prefix(buf, sizeof(buf)) << " docking failed" << std::endl;
            boinc_finish(1);
            return 1;
        }

        boinc_fraction_done(1.);
        boinc_finish(0);
    }
    catch (const std::exception& ex) {
        std::cerr << boinc_msg_prefix(buf, sizeof(buf)) << " exception was thrown while running: " << ex.what() << std::endl;
        boinc_finish(1);
        return 1;
    }

    return 0;
}
