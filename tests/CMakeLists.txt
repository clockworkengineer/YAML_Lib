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
        source/yaml/YAML_Lib_Tests_Create_Array.cpp
        source/yaml/YAML_Lib_Tests_Create_Complex.cpp
        source/yaml/YAML_Lib_Tests_Create_Dictionary.cpp
        source/document/YAML_Lib_Tests_Document.cpp
        source/parse/YAML_Lib_Tests_Parse.cpp
        source/parse/YAML_Lib_Tests_Parse_Array.cpp
        source/parse/YAML_Lib_Tests_Parse_Anchors.cpp
        source/parse/YAML_Lib_Tests_Parse_Dictionary.cpp
        source/parse/YAML_Lib_Tests_Parse_Boolean.cpp
        source/parse/YAML_Lib_Tests_Parse_Numeric.cpp
        source/parse/YAML_Lib_Tests_Parse_String.cpp
        source/parse/YAML_Lib_Tests_Parse_Comment.cpp
        source/parse/YAML_Lib_Tests_Parse_Misc.cpp
        source/parse/YAML_Lib_Tests_Parse_Null.cpp
        source/parse/YAML_Lib_Tests_Parse_Sequence_Mapping.cpp
        source/stringify/YAML_Lib_Tests_Stringify.cpp
        source/io/YAML_Lib_Tests_ISource_Buffer.cpp
        source/io/YAML_Lib_Tests_IDestination_Buffer.cpp
        source/io/YAML_Lib_Tests_IDestination_File.cpp
        source/io/YAML_Lib_Tests_ISource_File.cpp
        source/io/YAML_Lib_Tests_File_GetFormat.cpp
        source/io/YAML_Lib_Tests_File_FromFile.cpp
        source/io/YAML_Lib_Tests_File_ToFile.cpp
        source/number/YAML_Lib_Tests_Number_Assignment.cpp
        source/number/YAML_Lib_Tests_Number_Conversion.cpp
        source/number/YAML_Lib_Tests_Number_Notation.cpp
        source/traverse/YAML_Lib_Tests_Traverse.cpp
        source/ynode/YAML_Lib_Tests_YNode_Assignment.cpp
        source/ynode/YAML_Lib_Tests_YNode_Construct.cpp
        source/ynode/YAML_Lib_Tests_YNode_Indexing.cpp
        source/ynode/YAML_Lib_Tests_YNode_Reference.cpp
        source/misc/YAML_Lib_Tests_Helper.cpp)

add_executable(${TEST_EXECUTABLE} ${TEST_SOURCES})
target_include_directories(${TEST_EXECUTABLE} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${TEST_EXECUTABLE} ${YAML_LIBRARY_NAME} Catch2::Catch2WithMain)

add_test(NAME ${TEST_EXECUTABLE} COMMAND ${TEST_EXECUTABLE})

add_custom_command(TARGET ${TEST_EXECUTABLE} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/files files)
add_custom_command(TARGET ${TEST_EXECUTABLE} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/files ${PROJECT_BINARY_DIR}/files)