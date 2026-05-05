
cmake_minimum_required(VERSION 3.18)

project(YAML_Lib_Examples VERSION 1.1.0 DESCRIPTION "YAML_Lib example programs")

# Examples that require file I/O support
set(FILE_IO_EXAMPLES
  YAML_Analyze_File
  YAML_Config_Manager
  YAML_Fibonacci
  YAML_Files_To_Bencode
  YAML_Files_To_JSON
  YAML_Files_To_XML
  YAML_Nested_Structure_Demo
  YAML_Parse_File
  YAML_Record_Query
  YAML_Simple_Read_Write
)

# Find all example source files
file(GLOB EXAMPLE_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/source "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

foreach(EXAMPLE_FILE ${EXAMPLE_SOURCES})
  # Get the target name by stripping the .cpp extension
  get_filename_component(EXAMPLE_NAME ${EXAMPLE_FILE} NAME_WE)

  # Skip file-I/O-dependent examples when YAML_LIB_FILE_IO is disabled
  if(NOT YAML_LIB_FILE_IO AND EXAMPLE_NAME IN_LIST FILE_IO_EXAMPLES)
    continue()
  endif()

  add_executable(${EXAMPLE_NAME} source/${EXAMPLE_FILE})
  target_include_directories(${EXAMPLE_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/examples/include/external
    ${CMAKE_SOURCE_DIR}/examples/include
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
  )
  target_link_libraries(${EXAMPLE_NAME} PRIVATE YAML_Lib)

  # Optionally reuse precompiled headers if available
  if(TARGET YAML_Lib)
    target_precompile_headers(${EXAMPLE_NAME} REUSE_FROM YAML_Lib)
  endif()

  install(TARGETS ${EXAMPLE_NAME} DESTINATION bin)
endforeach()

# Copy example files after build (if needed by any example)
if(YAML_LIB_FILE_IO)
  add_custom_command(
    TARGET YAML_Parse_File POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/examples/files
    $<TARGET_FILE_DIR:YAML_Parse_File>/files
  )
endif()
