cmake_minimum_required(VERSION 4.0)
project(reclip VERSION 0.1.0)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/out/${CMAKE_BUILD_TYPE})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR}/gen)

configure_file (
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
    ${CMAKE_BINARY_DIR}/gen/version.h
)

if (CMAKE_CXX_COMPILER_ID MATCHES "^(Apple)?Clang$")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE null.cc)
else ()
    message(FATAL_ERROR "Not implemented")
endif()

# main targets
add_library(reclip_app_main null.cc)
add_library(reclip_core null.cc)
add_library(reclip_ui null.cc)
add_library(reclip_communication null.cc)
# main targets

target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    reclip_app_main
)

# tests
enable_testing()

add_executable(unit_tests null.cc)
target_link_libraries(unit_tests PUBLIC gtest_main gmock)
add_test(NAME unit_tests COMMAND unit_tests)

add_executable(server_integration_tests null.cc)
target_link_libraries(server_integration_tests PUBLIC gtest_main gmock)
add_test(NAME server_integration_tests COMMAND server_integration_tests)
# tests

add_subdirectory(third_party)
add_subdirectory(app_main)
add_subdirectory(base)
add_subdirectory(core)
add_subdirectory(ui)
add_subdirectory(communication)
add_subdirectory(resources)
