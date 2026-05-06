
cmake_minimum_required(VERSION 3.18)

project(YAML_Lib_Unit_Tests VERSION 1.1.0 DESCRIPTION "YAML C++ Library Catch Unit Tests" LANGUAGES CXX)

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
  source/parse/YAML_Lib_Tests_Parse_Unicode_Escapes.cpp
  source/parse/YAML_Lib_Tests_Parse_Tags.cpp
  source/parse/YAML_Lib_Tests_Parse_Directives.cpp
  source/parse/YAML_Lib_Tests_Parse_Timestamp.cpp
  source/parse/YAML_Lib_Tests_Parse_Anchors_Extended.cpp
  source/parse/YAML_Lib_Tests_Parse_SpecCompliance.cpp
  source/parse/YAML_Lib_Tests_Parse_ErrorHandling.cpp
  source/parse/YAML_Lib_Tests_Parse_YamlTestSuite.cpp
  source/parse/YAML_Lib_Tests_Parse_Collections.cpp
  source/stringify/YAML_Lib_Tests_Stringify.cpp
  source/stringify/YAML_Lib_Tests_Stringify_Bencode.cpp
  source/stringify/YAML_Lib_Tests_Stringify_JSON.cpp
  source/stringify/YAML_Lib_Tests_Stringify_XML.cpp
  source/stringify/YAML_Lib_Tests_Stringify_RoundTrip.cpp
  source/io/YAML_Lib_Tests_ISource_Buffer.cpp
  source/io/YAML_Lib_Tests_IDestination_Buffer.cpp
  source/io/YAML_Lib_Tests_IDestination_File.cpp
  source/io/YAML_Lib_Tests_ISource_File.cpp
  source/io/YAML_Lib_Tests_ISource_Stream.cpp
  source/io/YAML_Lib_Tests_IDestination_Stream.cpp
  source/io/YAML_Lib_Tests_File_GetFormat.cpp
  source/io/YAML_Lib_Tests_File_FromFile.cpp
  source/io/YAML_Lib_Tests_File_ToFile.cpp
  source/number/YAML_Lib_Tests_Number_Assignment.cpp
  source/number/YAML_Lib_Tests_Number_Conversion.cpp
  source/number/YAML_Lib_Tests_Number_Misc.cpp
  source/number/YAML_Lib_Tests_Number_Notation.cpp
  source/number/YAML_Lib_Tests_Number_Precision.cpp
  source/traverse/YAML_Lib_Tests_Traverse.cpp
  source/node/YAML_Lib_Tests_Node_Assignment.cpp
  source/node/YAML_Lib_Tests_Node_Construct.cpp
  source/node/YAML_Lib_Tests_Node_Indexing.cpp
  source/node/YAML_Lib_Tests_Node_Reference.cpp
  source/misc/YAML_Lib_Tests_Helper.cpp
  source/misc/YAML_Lib_Tests_Schema.cpp
  source/misc/YAML_Lib_Tests_Options.cpp
  source/misc/YAML_Lib_Tests_Phase3.cpp
  source/misc/YAML_Lib_Tests_SAX.cpp)

add_executable(${TEST_EXECUTABLE} ${TEST_SOURCES})
target_include_directories(${TEST_EXECUTABLE} PRIVATE
  ${CMAKE_SOURCE_DIR}/classes/include
  ${CMAKE_SOURCE_DIR}/classes/include/implementation
  ${CMAKE_SOURCE_DIR}/classes/include/interface
  ${CMAKE_SOURCE_DIR}/classes/include/implementation/common
  ${CMAKE_SOURCE_DIR}/classes/include/implementation/converter
  ${CMAKE_SOURCE_DIR}/classes/include/implementation/io
  ${CMAKE_SOURCE_DIR}/classes/include/implementation/parser
  ${CMAKE_SOURCE_DIR}/classes/include/implementation/variants
  ${CMAKE_SOURCE_DIR}/classes/include/implementation/node
  ${CMAKE_SOURCE_DIR}/classes/include/implementation/stringify
  ${CMAKE_SOURCE_DIR}/classes/include/implementation/translator
  ${CMAKE_SOURCE_DIR}/classes/include/implementation
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/tests/include
)
target_link_libraries(${TEST_EXECUTABLE} ${YAML_LIBRARY_NAME} Catch2::Catch2WithMain)
target_precompile_headers(${TEST_EXECUTABLE} REUSE_FROM YAML_Lib)
target_compile_definitions(${TEST_EXECUTABLE} PRIVATE
  YAML_SUITE_SRC_DIR="${CMAKE_SOURCE_DIR}/tests/yaml-test-suite/yaml-test-suite/src"
)

add_test(NAME ${TEST_EXECUTABLE} COMMAND ${TEST_EXECUTABLE})

add_custom_command(
  TARGET ${TEST_EXECUTABLE} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/tests/files
  $<TARGET_FILE_DIR:${TEST_EXECUTABLE}>/files
)