cmake_minimum_required(VERSION 3.18.1)

project("YAML_Lib" VERSION 0.0.1 DESCRIPTION "YAML C++ Library" LANGUAGES CXX)

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
)

# YAML includes
set(YAML_INCLUDES
  YAML_Config.hpp
  ./classes/include/YAML.hpp
)

# YAML library
add_library(${YAML_LIBRARY_NAME} ${YAML_SOURCES} ${YAML_INCLUDES})
target_include_directories(${YAML_LIBRARY_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}/classes/include
  ${PROJECT_BINARY_DIR})

# Add tests
# add_subdirectory(tests)

# Add examples
# add_subdirectory(examples)

# install
install(TARGETS ${YAML_LIBRARY_NAME} DESTINATION lib)
install(FILES ${YAML_INCLUDES} DESTINATION include)
