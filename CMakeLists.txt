cmake_minimum_required(VERSION 3.18)
project(YAML_Lib VERSION 1.3.0 DESCRIPTION "YAML C++ Library" LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform-specific options
if(MSVC)
  set(PLATFORM "windows")
else()
  set(PLATFORM "linux")
endif()

configure_file(YAML_Config.h.in YAML_Config.hpp)

option(BUILD_YAML_EXAMPLES "Build YAML_Lib example programs" OFF)
option(BUILD_YAML_TESTS "Build YAML_Lib unit tests" ON)
option(BUILD_YAML_PARSER_FUZZ_TESTS "Build parser fuzz harness tests" ON)

# E1: disable C++ exceptions (enables YAML_THROW / panic-handler path)
option(YAML_LIB_NO_EXCEPTIONS "Disable C++ exceptions; use error panic handler" OFF)

# E6: disable file I/O (fstream) for bare-metal / no-filesystem targets
option(YAML_LIB_FILE_IO "Include file I/O support (fromFile/toFile/getFileFormat)" OFF)

# E8: ISO 8601 Timestamp → time_t / struct tm conversion accessors
option(YAML_LIB_TIMESTAMP_PARSE "Include ISO 8601 Timestamp to time_t/tm conversion" OFF)

# E10: SAX-style (streaming) event-based parse API
option(YAML_LIB_SAX_API "Include SAX-style IYAMLEvents / traverseEvents() API" OFF)

enable_testing()

set(YAML_LIBRARY_NAME ${PROJECT_NAME})

# Sources
set(YAML_SOURCES
  classes/source/YAML.cpp
  classes/source/YAML_Utility.cpp
  classes/source/implementation/common/YAML_StringifierFactory.cpp
  classes/source/implementation/YAML_Error.cpp
  classes/source/implementation/YAML_Impl.cpp
  classes/source/implementation/YAML_Impl_Stringify.cpp
  classes/source/implementation/YAML_Impl_Access.cpp
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
  classes/source/implementation/converter/YAML_Converter.cpp
)

# Headers (public)
set(YAML_PUBLIC_HEADERS
  classes/include/YAML.hpp
  classes/include/YAML_Core.hpp
  classes/include/YAML_Reader.hpp
  classes/include/YAML_Writer.hpp
  classes/include/YAML_DOM.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/YAML_Config.hpp
)

add_library(${YAML_LIBRARY_NAME} ${YAML_SOURCES})
add_library(YAML_Lib::YAML_Lib ALIAS ${YAML_LIBRARY_NAME})

include(GenerateExportHeader)
generate_export_header(${YAML_LIBRARY_NAME}
  BASE_NAME YAML_Lib
  EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/YAML_Lib_Export.hpp"
)
list(APPEND YAML_PUBLIC_HEADERS "${CMAKE_CURRENT_BINARY_DIR}/YAML_Lib_Export.hpp")

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(YAML_LIB_IS_TOP_LEVEL ON)
else()
  set(YAML_LIB_IS_TOP_LEVEL OFF)
endif()

option(YAML_LIB_WERROR "Treat compiler warnings as errors" ${YAML_LIB_IS_TOP_LEVEL})

if(MSVC)
  target_compile_options(${YAML_LIBRARY_NAME} PRIVATE /W4)
  if(YAML_LIB_WERROR)
    target_compile_options(${YAML_LIBRARY_NAME} PRIVATE /WX)
  endif()
else()
  target_compile_options(${YAML_LIBRARY_NAME} PRIVATE -Wall -Wextra -pedantic)
  if(YAML_LIB_WERROR)
    target_compile_options(${YAML_LIBRARY_NAME} PRIVATE -Werror)
  endif()
endif()

target_include_directories(${YAML_LIBRARY_NAME}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/classes/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/classes/include/implementation>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/classes/include/implementation/common>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/classes/include/implementation/converter>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/classes/include/implementation/io>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/classes/include/implementation/parser>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/classes/include/implementation/stringify>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/classes/include/implementation/translator>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/classes/include/implementation/variants>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/classes/include/implementation/node>
)

target_precompile_headers(${YAML_LIBRARY_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h)

set(NO_EXCEPTIONS_COMPILE_TEST_EXECUTABLE ${YAML_LIBRARY_NAME}_NoExceptions_Compile_Tests)
add_executable(${NO_EXCEPTIONS_COMPILE_TEST_EXECUTABLE}
  tests/source/misc/YAML_Lib_Tests_NoExceptionsCompile.cpp)
target_include_directories(${NO_EXCEPTIONS_COMPILE_TEST_EXECUTABLE} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/classes/include
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/include
)
target_link_libraries(${NO_EXCEPTIONS_COMPILE_TEST_EXECUTABLE} PRIVATE ${YAML_LIBRARY_NAME})
add_dependencies(${NO_EXCEPTIONS_COMPILE_TEST_EXECUTABLE} ${YAML_LIBRARY_NAME})

if(YAML_LIB_NO_EXCEPTIONS)
  target_compile_definitions(${YAML_LIBRARY_NAME} PUBLIC YAML_LIB_NO_EXCEPTIONS)
  target_compile_options(${YAML_LIBRARY_NAME} PUBLIC -fno-exceptions)
endif()

if(YAML_LIB_FILE_IO)
  target_sources(${YAML_LIBRARY_NAME} PRIVATE
    classes/source/implementation/file/YAML_File.cpp
    classes/source/implementation/io/YAML_FileIO.cpp)
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

find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
  add_custom_target(docs
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
  )
endif()

install(TARGETS ${YAML_LIBRARY_NAME}
  EXPORT YAML_LibTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES ${YAML_PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${YAML_PUBLIC_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/YAML_Lib)

install(DIRECTORY classes/include/interface/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/interface
  FILES_MATCHING PATTERN "*.hpp"
)
install(DIRECTORY classes/include/interface/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/YAML_Lib/interface
  FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY classes/include/implementation/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/implementation
  FILES_MATCHING
  PATTERN "*.hpp"
  PATTERN "YAML_Impl.hpp" EXCLUDE
)
install(DIRECTORY classes/include/implementation/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/YAML_Lib/implementation
  FILES_MATCHING
  PATTERN "*.hpp"
  PATTERN "YAML_Impl.hpp" EXCLUDE
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/YAML_LibConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/YAML_LibConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/YAML_LibConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/YAML_Lib
)

install(EXPORT YAML_LibTargets
  FILE YAML_LibTargets.cmake
  NAMESPACE YAML_Lib::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/YAML_Lib
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/YAML_LibConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/YAML_LibConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/YAML_Lib
)

configure_file(cmake/yaml_lib.pc.in yaml_lib.pc @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/yaml_lib.pc"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)
