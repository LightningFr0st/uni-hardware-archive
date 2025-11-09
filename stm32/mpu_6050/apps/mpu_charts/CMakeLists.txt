cmake_minimum_required(VERSION 3.21)
project(mpu_charts LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_PREFIX_PATH 
    ${PROJECT_SOURCE_DIR}/../../libs/wxWidgets
)

find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

add_executable(mpu_charts WIN32 ${SOURCES} ${HEADERS})

target_link_libraries(mpu_charts PRIVATE
    mpu_sdk
    ${wxWidgets_LIBRARIES}
)

target_include_directories(mpu_charts PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_custom_command(TARGET mpu_charts POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:mpu_sdk>"
        $<TARGET_FILE_DIR:mpu_charts>)

add_custom_command(TARGET mpu_charts POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/../../libs/wxWidgets/lib/vc14x_x64_dll/wxmsw331ud_core_vc14x_x64.dll"
        $<TARGET_FILE_DIR:mpu_charts>)

add_custom_command(TARGET mpu_charts POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/../../libs/wxWidgets/lib/vc14x_x64_dll/wxbase331ud_vc14x_x64.dll"
        $<TARGET_FILE_DIR:mpu_charts>)
