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
    encoder(json_encoder)
{
}

bool json_encoder_helper::begin_object() const {
    return encoder.begin_object();
}

bool json_encoder_helper::begin_object(const std::string& key) const {
    if (encoder.key(key)) {
        return encoder.begin_object();
    }
    return false;
}

bool json_encoder_helper::end_object() const {
    return encoder.end_object();
}

bool json_encoder_helper::begin_array() const {
    return encoder.begin_array();
}

bool json_encoder_helper::begin_array(const std::string& key) const {
    if (encoder.key(key)) {
        return encoder.begin_array();
    }
    return false;
}

bool json_encoder_helper::end_array() const {
    return encoder.end_array();
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
