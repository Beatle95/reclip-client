cmake_minimum_required(VERSION 3.15)
project(reclip VERSION 0.0.1)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui)

if (CMAKE_CXX_COMPILER_ID MATCHES "^(Apple)?Clang$")
    add_compile_options(-fno-exceptions -Wall -Wextra -Wpedantic -Werror)
else ()
    message(FATAL_ERROR "Not implemented")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# main targets
add_executable(${PROJECT_NAME} main.cc)
add_library(reclip_core null.cc)
# main targets

# unit_tests
enable_testing()

add_executable(unit_tests null.cc)
target_link_libraries(unit_tests PUBLIC gtest_main reclip_core)
add_test(NAME unit_tests COMMAND unit_tests)
# unit_tests

target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    reclip_core
)

add_subdirectory(third_party)
add_subdirectory(base)
add_subdirectory(core)
add_subdirectory(ui)
add_subdirectory(communication)
