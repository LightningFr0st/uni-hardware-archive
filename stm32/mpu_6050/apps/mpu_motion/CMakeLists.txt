cmake_minimum_required(VERSION 3.21)
project(mpu_motion LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_PREFIX_PATH 
    ${PROJECT_SOURCE_DIR}/../../libs/SDL2
    ${PROJECT_SOURCE_DIR}/../../libs/SDL2_image
)

find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2 SDL2main)
find_package(SDL2_image REQUIRED)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

add_executable(mpu_motion WIN32 ${SOURCES} ${HEADERS})

target_link_libraries(mpu_motion PRIVATE
    mpu_sdk
    SDL2::SDL2
    SDL2::SDL2main
    SDL2_image::SDL2_image
)

target_include_directories(mpu_motion PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_custom_command(TARGET mpu_motion POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:mpu_sdk>"
        $<TARGET_FILE_DIR:mpu_motion>)

add_custom_command(TARGET mpu_motion POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/../../libs/SDL2/lib/x64/SDL2.dll"
        $<TARGET_FILE_DIR:mpu_motion>)

add_custom_command(TARGET mpu_motion POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/../../libs/SDL2_image/lib/x64/SDL2_image.dll"
        $<TARGET_FILE_DIR:mpu_motion>)
