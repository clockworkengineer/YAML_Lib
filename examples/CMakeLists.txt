cmake_minimum_required(VERSION 3.18.1)

project("YAML_Lib Examples" VERSION 1.1.0 DESCRIPTION "YAML_Lib example programs")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Get example program list

file(GLOB EXAMPLE_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/source/ "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp")

# Build each program

foreach(EXAMPLE_PROGRAM ${EXAMPLE_SOURCES})
  string(REPLACE ".cpp" "" EXAMPLE_TARGET ${EXAMPLE_PROGRAM})
  add_executable(${EXAMPLE_TARGET} ${CMAKE_CURRENT_SOURCE_DIR}/source/${EXAMPLE_PROGRAM}
          source/YAML_Files_To_Bencode.cpp
          source/YAML_Files_To_JSON.cpp)
  target_link_libraries(${EXAMPLE_TARGET} YAML_Lib)
  target_include_directories(${EXAMPLE_TARGET} PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/external)
  install(TARGETS ${EXAMPLE_TARGET} DESTINATION bin)
endforeach(EXAMPLE_PROGRAM ${EXAMPLE_SOURCES})

add_custom_command(TARGET YAML_Parse_File POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/files ${PROJECT_BINARY_DIR}/files)
