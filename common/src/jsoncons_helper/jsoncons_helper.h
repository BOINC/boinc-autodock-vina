// This file is part of BOINC.
// https://boinc.berkeley.edu
// Copyright (C) 2022 University of California
//
// BOINC is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// BOINC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with BOINC.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <stack>
#include <jsoncons/json.hpp>

class json_encoder_helper {
public:
    explicit json_encoder_helper(jsoncons::json_stream_encoder& json_encoder);
    ~json_encoder_helper() = default;

    bool begin_object();
    bool begin_object(const std::string& key);
    bool end_object();
    bool end_object(const std::string& key);

    bool begin_array();
    bool begin_array(const std::string& key);
    bool end_array();
    bool end_array(const std::string& key);

    bool value(const std::string& key, const char* value) const;
    bool value(const std::string& key, const std::string& value) const;
    bool value(const std::string& key, const double& value) const;
    bool value(const std::string& key, const uint64_t& value) const;
    bool value(const std::string& key, const int64_t& value) const;
    bool value(const std::string& key, const bool& value) const;

    bool value(const char* value) const;
    bool value(const std::string& value) const;
    bool value(const double& value) const;
    bool value(const uint64_t& value) const;
    bool value(const int64_t& value) const;
    bool value(const bool& value) const;

    [[nodiscard]] bool is_complete() const;
private:
    jsoncons::json_stream_encoder& encoder;

    std::stack<std::pair<std::string, std::string>> structure;
};
