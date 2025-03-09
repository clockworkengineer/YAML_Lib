cmake_minimum_required(VERSION 3.18.1)

project("YAML_Lib" VERSION 1.1.0 DESCRIPTION "YAML C++ Library" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  add_compile_options(/W4)
  set(PLATFORM "windows")
else()
  add_compile_options(-Wall -Werror -pedantic)
  set(PLATFORM "linux")
endif()

configure_file(YAML_Config.h.in YAML_Config.hpp)
 
set(YAML_LIBRARY_NAME ${PROJECT_NAME})

# YAML sources
set(YAML_SOURCES
  ./classes/source/YAML.cpp
  ./classes/source/implementation/YAML_Impl.cpp
  ./classes/source/implementation/file/YAML_File.cpp
  ./classes/source/implementation/parser/YAML_Parser.cpp
  ./classes/source/implementation/parser/YAML_Parser_Array.cpp
  ./classes/source/implementation/parser/YAML_Parser_BlockString.cpp
  ./classes/source/implementation/parser/YAML_Parser_Dictionary.cpp
  ./classes/source/implementation/parser/YAML_Parser_Directive.cpp
  ./classes/source/implementation/parser/YAML_Parser_FlowString.cpp
  ./classes/source/implementation/parser/YAML_Parser_Router.cpp
  ./classes/source/implementation/parser/YAML_Parser_Scalar.cpp
  ./classes/source/implementation/parser/YAML_Parser_Util.cpp
  ./classes/source/implementation/stringify/YAML_Stringify.cpp
  ./classes/source/implementation/translator/Default_Translator.cpp
)

list(APPEND YAML_SOURCES "./classes/source/implementation/converter/${PLATFORM}/YAML_Converter.cpp")

# YAML includes
set(YAML_INCLUDES
  YAML_Config.hpp
  ./classes/include/YAML.hpp
  ./classes/include/interface/IAction.hpp
  ./classes/include/interface/ISource.hpp
  ./classes/include/interface/IDestination.hpp
  ./classes/include/interface/IParser.hpp
  ./classes/include/interface/IStringify.hpp
  ./classes/include/interface/ITranslator.hpp
  ./classes/include/interface/YAML_Interfaces.hpp
  ./classes/include/implementation/common/YAML_Error.hpp
  ./classes/include/implementation/io/YAML_Sources.hpp
  ./classes/include/implementation/io/YAML_Destinations.hpp
  ./classes/include/implementation/stringify/YAML_Stringify.hpp
  ./classes/include/implementation/stringify/Bencode_Stringify.hpp
  ./classes/include/implementation/stringify/JSON_Stringify.hpp
  ./classes/include/implementation/stringify/XML_Stringify.hpp
  ./classes/include/implementation/parser/YAML_Parser.hpp
  ./classes/include/implementation/converter/YAML_Converter.hpp
  ./classes/include/implementation/translator/XML_Translator.hpp
  ./classes/include/implementation/translator/Default_Translator.hpp
  ./classes/include/implementation/variants/YAML_Hole.hpp
  ./classes/include/implementation/variants/YAML_Array.hpp
  ./classes/include/implementation/variants/YAML_Boolean.hpp
  ./classes/include/implementation/variants/YAML_Comment.hpp
  ./classes/include/implementation/variants/YAML_Document.hpp
  ./classes/include/implementation/variants/YAML_Null.hpp
  ./classes/include/implementation/variants/YAML_Number.hpp
  ./classes/include/implementation/variants/YAML_Dictionary.hpp
  ./classes/include/implementation/variants/YAML_String.hpp
  ./classes/include/implementation/ynode/YAML_YNode.hpp
  ./classes/include/implementation/ynode/YAML_YNode_Creation.hpp
  ./classes/include/implementation/ynode/YAML_YNode_Index.hpp
  ./classes/include/implementation/ynode/YAML_YNode_Reference.hpp
  ./classes/include/implementation/YAML_Impl.hpp
  ./classes/include/implementation/YAML_Core.hpp
)

# YAML library
add_library(${YAML_LIBRARY_NAME} ${YAML_SOURCES} ${YAML_INCLUDES}) 
target_include_directories(${YAML_LIBRARY_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}/classes/include
  ${PROJECT_SOURCE_DIR}/classes/include/implementation
  ${PROJECT_SOURCE_DIR}/classes/include/implementation/common
  ${PROJECT_SOURCE_DIR}/classes/include/implementation/converter
  ${PROJECT_SOURCE_DIR}/classes/include/implementation/io
  ${PROJECT_SOURCE_DIR}/classes/include/implementation/parser
  ${PROJECT_SOURCE_DIR}/classes/include/implementation/variants
  ${PROJECT_SOURCE_DIR}/classes/include/implementation/ynode
  ${PROJECT_SOURCE_DIR}/classes/include/implementation/parser
  ${PROJECT_SOURCE_DIR}/classes/include/implementation/stringify
  ${PROJECT_SOURCE_DIR}/classes/include/implementation/translator
  ${PROJECT_SOURCE_DIR}/classes/include/interface
  ${PROJECT_BINARY_DIR})

# Add tests
add_subdirectory(tests)

# Add examples
 add_subdirectory(examples)

# install
install(TARGETS ${YAML_LIBRARY_NAME} DESTINATION lib)
install(FILES ${YAML_INCLUDES} DESTINATION include)
