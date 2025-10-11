cmake_minimum_required(VERSION 3.5)
project(com_chart)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_PREFIX_PATH 
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/wxWidgets
)

find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})

if(wxWidgets_USE_FILE)
    include(${wxWidgets_USE_FILE})
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

add_executable(com_chart WIN32 ${SOURCES} ${HEADERS})

target_link_libraries(com_chart PRIVATE ${wxWidgets_LIBRARIES})

add_custom_command(TARGET com_chart POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/libs/wxWidgets//lib/vc14x_x64_dll/wxmsw331ud_core_vc14x_x64.dll"
        $<TARGET_FILE_DIR:com_chart>)

add_custom_command(TARGET com_chart POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/libs/wxWidgets/lib/vc14x_x64_dll/wxbase331ud_vc14x_x64.dll"
        $<TARGET_FILE_DIR:com_chart>)