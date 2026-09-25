from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.files import copy, get
import os

class YamlLibConan(ConanFile):
    name = "yaml_lib"
    version = "1.3.0"
    license = "MIT"
    author = "Rob Turner"
    url = "https://github.com/clockworkengineer/YAML_Lib"
    description = "Fast, modern C++20 YAML library with DOM and JSON/YAML conversion support"
    topics = ("yaml", "json", "parser", "cpp20", "dom")
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "file_io": [True, False],
        "sax_api": [True, False],
        "timestamp_parse": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "file_io": True,
        "sax_api": False,
        "timestamp_parse": False,
    }

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["BUILD_YAML_TESTS"] = False
        tc.variables["BUILD_YAML_EXAMPLES"] = False
        tc.variables["YAML_LIB_FILE_IO"] = self.options.file_io
        tc.variables["YAML_LIB_SAX_API"] = self.options.sax_api
        tc.variables["YAML_LIB_TIMESTAMP_PARSE"] = self.options.timestamp_parse
        tc.variables["BUILD_SHARED_LIBS"] = self.options.shared
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["YAML_Lib"]
        self.cpp_info.set_property("cmake_file_name", "YAML_Lib")
        self.cpp_info.set_property("cmake_target_name", "YAML_Lib::YAML_Lib")
        self.cpp_info.set_property("pkg_config_name", "yaml_lib")
