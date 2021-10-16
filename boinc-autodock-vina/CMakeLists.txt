cmake_minimum_required(VERSION 3.15)

project (boinc-autodock-vina CXX)

find_package(autodock-vina REQUIRED)
find_package(boinc REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem log math_tr1 program_options random serialization thread timer)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_executable(boinc-autodock-vina src/main.cpp)

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
    )
