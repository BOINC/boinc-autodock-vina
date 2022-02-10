# This file is part of BOINC.
# https://boinc.berkeley.edu
# Copyright (C) 2022 University of California
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
find_package(libzip CONFIG REQUIRED)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (UNIX)
    set(CMAKE_CXX_FLAGS "-fpermissive")
endif()

add_library(config
    STATIC
        src/common/config.h
        src/common/config.cpp
)

add_library(zip-helper
    STATIC
        src/common/zip-extract.h
        src/common/zip-extract.cpp
        src/common/zip-create.h
        src/common/zip-create.cpp
)

if (BUILD_WORK_GENERATOR)
    add_library(input-config
        STATIC
            src/work-generator/input-config.h
            src/work-generator/input-config.cpp
            src/work-generator/temp-folder.h
            src/work-generator/temp-folder.cpp
    )
endif()

add_library(calculate
    STATIC
        src/boinc-autodock-vina/calculate.h
        src/boinc-autodock-vina/calculate.cpp
)

add_library(jsoncons_helper
    STATIC
        ../common/src/jsoncons_helper/jsoncons_helper.h
        ../common/src/jsoncons_helper/jsoncons_helper.cpp
)

if (NOT COVERAGE_REPORT)
    add_executable(boinc-autodock-vina
        src/boinc-autodock-vina/boinc-autodock-vina.cpp
    )
endif()

if (NOT COVERAGE_REPORT)
    add_executable(config-validator
        src/config-validator/config-validator.cpp
    )
endif()

add_executable(unit-tests
    src/unit-tests/config-tests.cpp
    src/unit-tests/dummy-ofstream.h
    src/unit-tests/dummy-ofstream.cpp
)

if (BUILD_WORK_GENERATOR)
    target_sources(unit-tests
        PRIVATE
            src/unit-tests/input-config-tests.cpp
    )
endif()

if (BUILD_WORK_GENERATOR AND NOT COVERAGE_REPORT)
    add_executable(work-generator
        src/work-generator/work-generator.cpp
    )
endif()

if (COVERAGE_REPORT)
    target_compile_options(unit-tests PRIVATE -g -O0 --coverage -fprofile-abs-path)
    target_compile_options(config PRIVATE -g -O0 --coverage -fprofile-abs-path)
    target_compile_options(input-config PRIVATE -g -O0 --coverage -fprofile-abs-path)
    target_compile_options(zip-helper PRIVATE -g -O0 --coverage -fprofile-abs-path)
    target_compile_options(calculate PRIVATE -g -O0 --coverage -fprofile-abs-path)
    target_compile_options(jsoncons_helper PRIVATE -g -O0 --coverage -fprofile-abs-path)

    target_link_options(unit-tests PRIVATE --coverage)
    target_link_options(config PRIVATE --coverage)
    target_link_options(input-config PRIVATE --coverage)
    target_link_options(zip-helper PRIVATE --coverage)
    target_link_options(calculate PRIVATE --coverage)
    target_link_options(jsoncons_helper PRIVATE --coverage)
endif()

set(BOINC_AUTODOCK_VINA_VERSION "1.0.0")
if (NOT DEFINED BOINC_APPS_GIT_REVISION)
    set(BOINC_APPS_GIT_REVISION "unknown")
endif()

if (NOT COVERAGE_REPORT)
    target_compile_definitions(boinc-autodock-vina
        PRIVATE
            -DBOINC_AUTODOCK_VINA_VERSION="${BOINC_AUTODOCK_VINA_VERSION}"
            -DBOINC_APPS_GIT_REVISION="${BOINC_APPS_GIT_REVISION}"
    )
endif()

if (NOT COVERAGE_REPORT)
    target_compile_definitions(config-validator
        PRIVATE
            -DBOINC_AUTODOCK_VINA_VERSION="${BOINC_AUTODOCK_VINA_VERSION}"
            -DBOINC_APPS_GIT_REVISION="${BOINC_APPS_GIT_REVISION}"
    )
endif()

target_compile_definitions(unit-tests
    PRIVATE
        -DBOINC_AUTODOCK_VINA_VERSION="${BOINC_AUTODOCK_VINA_VERSION}"
        -DBOINC_APPS_GIT_REVISION="${BOINC_APPS_GIT_REVISION}"
)

if (BUILD_WORK_GENERATOR AND NOT COVERAGE_REPORT)
    target_compile_definitions(work-generator
        PRIVATE
            -DBOINC_AUTODOCK_VINA_VERSION="${BOINC_AUTODOCK_VINA_VERSION}"
            -DBOINC_APPS_GIT_REVISION="${BOINC_APPS_GIT_REVISION}"
    )
endif()

target_compile_definitions(config
    PRIVATE
        -DBOINC_AUTODOCK_VINA_VERSION="${BOINC_AUTODOCK_VINA_VERSION}"
        -DBOINC_APPS_GIT_REVISION="${BOINC_APPS_GIT_REVISION}"
)

if (BUILD_WORK_GENERATOR)
    target_compile_definitions(input-config
        PRIVATE
            -DBOINC_AUTODOCK_VINA_VERSION="${BOINC_AUTODOCK_VINA_VERSION}"
            -DBOINC_APPS_GIT_REVISION="${BOINC_APPS_GIT_REVISION}"
    )
endif()

target_compile_definitions(calculate
    PRIVATE
        -DBOINC_AUTODOCK_VINA_VERSION="${BOINC_AUTODOCK_VINA_VERSION}"
        -DBOINC_APPS_GIT_REVISION="${BOINC_APPS_GIT_REVISION}"
)

target_compile_definitions(jsoncons_helper
    PRIVATE
        -DBOINC_AUTODOCK_VINA_VERSION="${BOINC_AUTODOCK_VINA_VERSION}"
        -DBOINC_APPS_GIT_REVISION="${BOINC_APPS_GIT_REVISION}"
)

if (NOT COVERAGE_REPORT)
    target_include_directories(boinc-autodock-vina
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/src
            ${CMAKE_CURRENT_LIST_DIR}/../common/src
    )
endif()

if (NOT COVERAGE_REPORT)
    target_include_directories(config-validator
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/src
            ${CMAKE_CURRENT_LIST_DIR}/../common/src
    )
endif()

target_include_directories(unit-tests
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src
        ${CMAKE_CURRENT_LIST_DIR}/../common/src
)

if (BUILD_WORK_GENERATOR AND NOT COVERAGE_REPORT)
    target_include_directories(work-generator
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/src
            ${CMAKE_CURRENT_LIST_DIR}/../common/src
    )
endif()

target_include_directories(config
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src
        ${CMAKE_CURRENT_LIST_DIR}/../common/src
)

target_include_directories(zip-helper
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src
        ${CMAKE_CURRENT_LIST_DIR}/../common/src
)

if (BUILD_WORK_GENERATOR)
    target_include_directories(input-config
        PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}/src
            ${CMAKE_CURRENT_LIST_DIR}/../common/src
    )
endif()

target_include_directories(calculate
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src
        ${CMAKE_CURRENT_LIST_DIR}/../common/src
)

target_include_directories(jsoncons_helper
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/../common/src
)

if (NOT COVERAGE_REPORT)
    target_link_libraries(boinc-autodock-vina
        PRIVATE
            config
            calculate
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
            jsoncons_helper
            zip-helper
            libzip::zip
    )
endif()

if (NOT COVERAGE_REPORT)
    target_link_libraries(config-validator
        PRIVATE
            config
            magic_enum::magic_enum
            jsoncons
            jsoncons_helper
            zip-helper
    )
endif()

set (UNIT_TEST_LINK_LIBRARIES
    config
    calculate
    jsoncons_helper
    GTest::gtest
    GTest::gtest_main
    autodock-vina::autodock-vina::vina
    autodock-vina::autodock-vina::vina_split
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
    zip-helper
    libzip::zip
)

if (BUILD_WORK_GENERATOR)
    set (UNIT_TEST_LINK_LIBRARIES
        ${UNIT_TEST_LINK_LIBRARIES}
        input-config
    )
endif()

target_link_libraries(unit-tests
    PRIVATE
        ${UNIT_TEST_LINK_LIBRARIES}
)

if (BUILD_WORK_GENERATOR AND NOT COVERAGE_REPORT)
    target_link_libraries(work-generator
        PRIVATE
            config
            input-config
            jsoncons_helper
            magic_enum::magic_enum
            jsoncons
            libzip::zip
            Boost::filesystem
    )
endif()

target_link_libraries(config
    PRIVATE
       jsoncons
       jsoncons_helper
)

target_link_libraries(zip-helper
    PRIVATE
        libzip::zip
)

if (BUILD_WORK_GENERATOR)
    target_link_libraries(input-config
        PRIVATE
            config
            jsoncons
            zip-helper
            libzip::zip
    )
endif()

target_link_libraries(calculate
    PRIVATE
       jsoncons
)

target_link_libraries(jsoncons_helper
    PRIVATE
       jsoncons
)
