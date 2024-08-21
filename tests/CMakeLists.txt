cmake_minimum_required(VERSION 3.18.1)

project("YAML_Lib Unit Tests" VERSION 1.1.0 DESCRIPTION "YAML C++ Library Catch Unit Tests" LANGUAGES CXX)

# Get Catch2 from repository
include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.6.0
)

FetchContent_MakeAvailable(Catch2)

# Enable unit tests
enable_testing()

set(TEST_EXECUTABLE ${YAML_LIBRARY_NAME}_Unit_Tests)

set(TEST_SOURCES
        source/yaml/YAML_Lib_Tests_YAML.cpp
        source/parse/YAML_Lib_Tests_YAML_Parse.cpp
        source/parse/YAML_Lib_Tests_YAML_Parse_Array.cpp
        source/parse/YAML_Lib_Tests_YAML_Parse_Object.cpp
        source/parse/YAML_Lib_Tests_YAML_Parse_Simple.cpp
        source/parse/YAML_Lib_Tests_YAML_Parse_Comment.cpp
        source/io/YAML_Lib_Tests_ISource_Buffer.cpp
        source/io/YAML_Lib_Tests_IDestination_Buffer.cpp
        source/misc/YAML_Lib_Tests_Helper.cpp)

add_executable(${TEST_EXECUTABLE} ${TEST_SOURCES})
target_include_directories(${TEST_EXECUTABLE} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${TEST_EXECUTABLE} ${YAML_LIBRARY_NAME} Catch2::Catch2WithMain)

add_test(NAME ${TEST_EXECUTABLE} COMMAND ${TEST_EXECUTABLE})

add_custom_command(TARGET ${TEST_EXECUTABLE} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/files files)
add_custom_command(TARGET ${TEST_EXECUTABLE} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/files ${PROJECT_BINARY_DIR}/files)