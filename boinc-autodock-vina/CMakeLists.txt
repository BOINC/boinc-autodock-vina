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

if (UNIX)
    set (BUILD_FLAGS_FOR_CXX "-g -O0 --coverage -fprofile-abs-path")
    set (BUILD_FLAGS_FOR_C "-g -O0 --coverage -fprofile-abs-path")
    target_compile_options(unit-tests PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:${BUILD_FLAGS_FOR_CXX}>
        $<$<COMPILE_LANGUAGE:C>:${BUILD_FLAGS_FOR_C}>)"")
    target_link_options(unit-tests PRIVATE "--coverage")
endif()

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
