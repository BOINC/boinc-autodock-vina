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

#include "jsoncons_helper.h"

json_encoder_helper::json_encoder_helper(jsoncons::json_stream_encoder& json_encoder) :
    encoder(json_encoder) {}

bool json_encoder_helper::begin_object() {
    if (encoder.begin_object()) {
        structure.push({ "object", "" });
        return true;
    }
    return false;
}

//bool json_encoder_helper::begin_object(const char* key) {
//    return begin_object(std::string(key));
//}

bool json_encoder_helper::begin_object(const std::string& key) {
    if (encoder.key(key) && encoder.begin_object()) {
        structure.push({ "object", key });
        return true;
    }
    return false;
}

bool json_encoder_helper::end_object() {
    const auto& [type, name] = structure.top();
    if (type == "object" && name.empty() && encoder.end_object()) {
        structure.pop();
        return true;
    }
    return false;
}

//bool json_encoder_helper::end_object(const char* key) {
//    return end_object(std::string(key));
//}

bool json_encoder_helper::end_object(const std::string& key) {
    const auto& [type, name] = structure.top();
    if (type == "object" && name == key && encoder.end_object()) {
        structure.pop();
        return true;
    }
    return false;
}

bool json_encoder_helper::begin_array() {
    if (encoder.begin_array()) {
        structure.push({ "array", "" });
        return true;
    }
    return false;
}

//bool json_encoder_helper::begin_array(const char* key) {
//    return begin_array(std::string(key));
//}

bool json_encoder_helper::begin_array(const std::string& key) {
    if (encoder.key(key) && encoder.begin_array()) {
        structure.push({ "array", key });
        return true;
    }
    return false;
}

bool json_encoder_helper::end_array() {
    const auto& [type, name] = structure.top();
    if (type == "array" && name.empty() && encoder.end_array()) {
        structure.pop();
        return true;
    }
    return false;
}

//bool json_encoder_helper::end_array(const char* key) {
//    return end_array(std::string(key));
//}

bool json_encoder_helper::end_array(const std::string& key) {
    const auto& [type, name] = structure.top();
    if (type == "array" && name == key && encoder.end_array()) {
        structure.pop();
        return true;
    }
    return false;
}

bool json_encoder_helper::value(const std::string& key, const char* value) const {
    if (encoder.key(key)) {
        return encoder.string_value(value);
    }
    return false;
}

bool json_encoder_helper::value(const std::string& key, const std::string& value) const {
    if (encoder.key(key)) {
        return encoder.string_value(value);
    }
    return false;
}

bool json_encoder_helper::value(const std::string& key, const double& value) const {
    if (encoder.key(key)) {
        return encoder.double_value(value);
    }
    return false;
}

bool json_encoder_helper::value(const std::string& key, const uint64_t& value) const {
    if (encoder.key(key)) {
        return encoder.uint64_value(value);
    }
    return false;
}

bool json_encoder_helper::value(const std::string& key, const int64_t& value) const {
    if (encoder.key(key)) {
        return encoder.int64_value(value);
    }
    return false;
}

bool json_encoder_helper::value(const std::string& key, const bool& value) const {
    if (encoder.key(key)) {
        return encoder.bool_value(value);
    }
    return false;
}

bool json_encoder_helper::value(const char* value) const {
    return encoder.string_value(value);
}

bool json_encoder_helper::value(const std::string& value) const {
    return encoder.string_value(value);
}

bool json_encoder_helper::value(const double& value) const {
    return encoder.double_value(value);
}

bool json_encoder_helper::value(const uint64_t& value) const {
    return encoder.uint64_value(value);
}

bool json_encoder_helper::value(const int64_t& value) const {
    return encoder.int64_value(value);
}

bool json_encoder_helper::value(const bool& value) const {
    return encoder.bool_value(value);
}

bool json_encoder_helper::is_complete() const {
    return structure.empty();
}
