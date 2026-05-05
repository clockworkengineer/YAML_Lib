
cmake_minimum_required(VERSION 3.18)
project(YAML_Lib VERSION 1.1.0 DESCRIPTION "YAML C++ Library" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform-specific options
if(MSVC)
  set(PLATFORM "windows")
  add_compile_options(/W4)
else()
  set(PLATFORM "linux")
  add_compile_options(-Wall -Werror -pedantic)
endif()

configure_file(YAML_Config.h.in YAML_Config.hpp)

option(BUILD_YAML_EXAMPLES "Build YAML_Lib example programs" ON)
option(BUILD_YAML_TESTS "Build YAML_Lib unit tests" ON)

# E1: disable C++ exceptions (enables YAML_THROW / panic-handler path)
option(YAML_LIB_NO_EXCEPTIONS "Disable C++ exceptions; use error panic handler" OFF)

# E6: disable file I/O (fstream) for bare-metal / no-filesystem targets
option(YAML_LIB_FILE_IO "Include file I/O support (fromFile/toFile/getFileFormat)" ON)

# E8: ISO 8601 Timestamp → time_t / struct tm conversion accessors
option(YAML_LIB_TIMESTAMP_PARSE "Include ISO 8601 Timestamp to time_t/tm conversion" ON)

# E10: SAX-style (streaming) event-based parse API
option(YAML_LIB_SAX_API "Include SAX-style IYAMLEvents / traverseEvents() API" ON)

set(YAML_LIBRARY_NAME ${PROJECT_NAME})

# Sources
set(YAML_SOURCES
  classes/source/YAML.cpp
  classes/source/implementation/YAML_Error.cpp
  classes/source/implementation/YAML_Impl.cpp
  classes/source/implementation/parser/YAML_Parser.cpp
  classes/source/implementation/parser/YAML_Parser_Array.cpp
  classes/source/implementation/parser/YAML_Parser_BlockString.cpp
  classes/source/implementation/parser/YAML_Parser_Dictionary.cpp
  classes/source/implementation/parser/YAML_Parser_Directive.cpp
  classes/source/implementation/parser/YAML_Parser_FlowString.cpp
  classes/source/implementation/parser/YAML_Parser_Router.cpp
  classes/source/implementation/parser/YAML_Parser_Scalar.cpp
  classes/source/implementation/parser/YAML_Parser_Tag.cpp
  classes/source/implementation/parser/YAML_Parser_Timestamp.cpp
  classes/source/implementation/parser/YAML_Parser_Util.cpp
  classes/source/implementation/translator/Default_Translator.cpp
  classes/source/implementation/converter/${PLATFORM}/YAML_Converter.cpp
)

# Headers (public)
set(YAML_PUBLIC_HEADERS
  classes/include/YAML.hpp
  classes/include/YAML_Core.hpp
  classes/include/interface/IAction.hpp
  classes/include/interface/ISource.hpp
  classes/include/interface/IDestination.hpp
  classes/include/interface/IParser.hpp
  classes/include/interface/IStringify.hpp
  classes/include/interface/ITranslator.hpp
  classes/include/interface/YAML_Interfaces.hpp
  classes/include/implementation/common/YAML_Error.hpp
  classes/include/implementation/io/YAML_Sources.hpp
  classes/include/implementation/io/YAML_Destinations.hpp
  classes/include/implementation/stringify/Default_Stringify.hpp
  classes/include/implementation/stringify/Bencode_Stringify.hpp
  classes/include/implementation/stringify/JSON_Stringify.hpp
  classes/include/implementation/stringify/XML_Stringify.hpp
  classes/include/implementation/parser/Default_Parser.hpp
  classes/include/implementation/converter/YAML_Converter.hpp
  classes/include/implementation/translator/XML_Translator.hpp
  classes/include/implementation/translator/Default_Translator.hpp
  classes/include/implementation/variants/YAML_Hole.hpp
  classes/include/implementation/variants/YAML_Array.hpp
  classes/include/implementation/variants/YAML_Boolean.hpp
  classes/include/implementation/variants/YAML_Comment.hpp
  classes/include/implementation/variants/YAML_Document.hpp
  classes/include/implementation/variants/YAML_Null.hpp
  classes/include/implementation/variants/YAML_Number.hpp
  classes/include/implementation/variants/YAML_Dictionary.hpp
  classes/include/implementation/variants/YAML_String.hpp
  classes/include/implementation/node/YAML_Node.hpp
  classes/include/implementation/node/YAML_Node_Creation.hpp
  classes/include/implementation/node/YAML_Node_Index.hpp
  classes/include/implementation/node/YAML_Node_Reference.hpp
  classes/include/implementation/YAML_Impl.hpp
)

add_library(${YAML_LIBRARY_NAME} STATIC ${YAML_SOURCES})
target_include_directories(${YAML_LIBRARY_NAME}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/classes/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/classes/include/implementation>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/classes/include/interface>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/classes/include/implementation/common>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/classes/include/implementation/converter>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/classes/include/implementation/io>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/classes/include/implementation/parser>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/classes/include/implementation/variants>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/classes/include/implementation/node>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/classes/include/implementation/stringify>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/classes/include/implementation/translator>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/classes/include/implementation>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:include>
)

target_precompile_headers(${YAML_LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h)

if(YAML_LIB_NO_EXCEPTIONS)
  target_compile_definitions(${YAML_LIBRARY_NAME} PUBLIC YAML_LIB_NO_EXCEPTIONS)
  target_compile_options(${YAML_LIBRARY_NAME} PUBLIC -fno-exceptions)
endif()

if(YAML_LIB_FILE_IO)
  target_sources(${YAML_LIBRARY_NAME} PRIVATE
    classes/source/implementation/file/YAML_File.cpp)
  target_compile_definitions(${YAML_LIBRARY_NAME} PUBLIC YAML_LIB_FILE_IO)
endif()

if(YAML_LIB_TIMESTAMP_PARSE)
  target_compile_definitions(${YAML_LIBRARY_NAME} PUBLIC YAML_LIB_TIMESTAMP_PARSE)
endif()

if(YAML_LIB_SAX_API)
  target_compile_definitions(${YAML_LIBRARY_NAME} PUBLIC YAML_LIB_SAX_API)
endif()

if(BUILD_YAML_TESTS)
  add_subdirectory(tests)
endif()

if(BUILD_YAML_EXAMPLES)
  add_subdirectory(examples)
endif()

install(TARGETS ${YAML_LIBRARY_NAME}
  EXPORT YAML_LibTargets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
install(FILES ${YAML_PUBLIC_HEADERS} DESTINATION include)
install(EXPORT YAML_LibTargets
  FILE YAML_LibTargets.cmake
  NAMESPACE YAML_Lib::
  DESTINATION lib/cmake/YAML_Lib
)
