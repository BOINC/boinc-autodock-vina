# This file is part of BOINC.
# https://boinc.berkeley.edu
# Copyright (C) 2021 University of California
#
# BOINC is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License
# as published by the Free Software Foundation,
# either version 3 of the License, or (at your option) any later version.
#
# BOINC is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with BOINC.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.15)

project (boinc-autodock-vina CXX)

find_package(autodock-vina REQUIRED)
find_package(boinc REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem log math_tr1 program_options random serialization thread timer)
find_package(magic_enum CONFIG REQUIRED)
find_package(jsoncons CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (UNIX)
    set(CMAKE_CXX_FLAGS "-fpermissive")
endif()

add_executable(boinc-autodock-vina
     src/boinc-autodock-vina/boinc-autodock-vina.cpp
     src/boinc-autodock-vina/calculate.h
     src/boinc-autodock-vina/calculate.cpp
     src/common/config.h
     src/common/config.cpp
)

add_executable(config-validator
    src/config-validator/config-validator.cpp
    src/common/config.h
    src/common/config.cpp
)

add_executable(unit-tests
    src/common/config.h
    src/common/config.cpp
    src/boinc-autodock-vina/calculate.h
    src/boinc-autodock-vina/calculate.cpp
    src/unit-tests/unit-tests.cpp
)

if (UNIX AND CMAKE_CXX_COMPILER_ID MATCHES "GCC")
    target_compile_options(unit-tests PRIVATE -g -O0 --coverage -fprofile-abs-path)
    target_link_options(unit-tests PRIVATE --coverage)
endif()

if (APPLE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(unit-tests PRIVATE -g -O0 -fcoverage-mapping -fprofile-instr-generate)
    target_link_options(unit-tests PRIVATE -fcoverage-mapping -fprofile-instr-generate)
endif()

set(BOINC_AUTODOCK_VINA_VERSION "0.1.0")
if (NOT DEFINED BOINC_APPS_GIT_REVISION)
    set(BOINC_APPS_GIT_REVISION "unknown")
endif()

target_compile_definitions(boinc-autodock-vina
    PRIVATE
    -DBOINC_AUTODOCK_VINA_VERSION="${BOINC_AUTODOCK_VINA_VERSION}"
    -DBOINC_APPS_GIT_REVISION="${BOINC_APPS_GIT_REVISION}"
)

target_compile_definitions(config-validator
    PRIVATE
    -DBOINC_AUTODOCK_VINA_VERSION="${BOINC_AUTODOCK_VINA_VERSION}"
    -DBOINC_APPS_GIT_REVISION="${BOINC_APPS_GIT_REVISION}"
)

target_compile_definitions(unit-tests
    PRIVATE
    -DBOINC_AUTODOCK_VINA_VERSION="${BOINC_AUTODOCK_VINA_VERSION}"
    -DBOINC_APPS_GIT_REVISION="${BOINC_APPS_GIT_REVISION}"
)

target_include_directories(boinc-autodock-vina
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src
)

target_include_directories(config-validator
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src
)

target_include_directories(unit-tests
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src
)

target_link_libraries(boinc-autodock-vina
    PRIVATE
        unofficial::boinc::boinc
        unofficial::boinc::boincapi
        autodock-vina::autodock-vina::vina
        autodock-vina::autodock-vina::vina_split
        OpenSSL::SSL
        OpenSSL::Crypto
        Boost::boost
        Boost::filesystem
        Boost::log
        Boost::math_tr1
        Boost::program_options
        Boost::random
        Boost::serialization
        Boost::thread
        Boost::timer
        magic_enum::magic_enum
        jsoncons
)

target_link_libraries(config-validator
    PRIVATE
        magic_enum::magic_enum
        jsoncons
)

target_link_libraries(unit-tests
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        autodock-vina::autodock-vina::vina
        autodock-vina::autodock-vina::vina_split
        OpenSSL::SSL
        OpenSSL::Crypto
        Boost::boost
        Boost::filesystem
        Boost::log
        Boost::math_tr1
        Boost::program_options
        Boost::random
        Boost::serialization
        Boost::thread
        Boost::timer
        magic_enum::magic_enum
        jsoncons
)
