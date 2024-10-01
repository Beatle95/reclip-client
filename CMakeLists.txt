cmake_minimum_required(VERSION 3.15)
project(reclip VERSION 0.0.1)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_CXX_COMPILER_ID MATCHES "^(Apple)?Clang$")
    add_compile_options(-fno-exceptions -Wall -Wextra -Wpedantic -Werror)
else ()
    message(FATAL_ERROR "Not implemented")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${PROJECT_NAME} main.cc)

add_subdirectory(third_party)
add_subdirectory(core)
add_subdirectory(ui)

target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    reclip_core
)
