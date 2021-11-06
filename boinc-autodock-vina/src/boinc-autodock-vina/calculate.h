#pragma once

#include <filesystem>
#include <functional>

[[nodiscard]] bool calculate(const std::filesystem::path& json, const std::function<void(double)>& progress_callback);
