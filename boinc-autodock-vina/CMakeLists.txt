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

add_executable(boinc-autodock-vina
     src/boinc-autodock-vina.cpp
     src/config.h
     src/config.cpp
     src/config-loader.h
     src/config-loader.cpp
)

add_executable(config-validator
    src/config-validator.cpp
    src/config.h
    src/config.cpp
    src/config-loader.h
    src/config-loader.cpp
)

add_executable(unit-tests
    src/config.h
    src/config.cpp
    src/unit-tests.cpp
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
)
