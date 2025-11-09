cmake_minimum_required(VERSION 3.21)
project(mpu_sdk LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SDK_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB SDK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(mpu_sdk SHARED ${SDK_SOURCES} ${SDK_HEADERS})

target_include_directories(mpu_sdk
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(mpu_sdk PRIVATE MPUSDK_BUILD)
target_compile_definitions(mpu_sdk PUBLIC NOMINMAX WIN32_LEAN_AND_MEAN)