cmake_minimum_required(VERSION 3.21)

project(mpu_suite LANGUAGES CXX)

# Единые настройки вывода артефактов
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(sdk)
add_subdirectory(apps/mpu_motion)
add_subdirectory(apps/mpu_charts)