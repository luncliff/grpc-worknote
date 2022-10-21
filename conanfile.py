# https://docs.conan.io/en/latest/creating_packages/getting_started.html
from genericpath import exists
from conans import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake
from conan.tools.layout import cmake_layout
from os.path import join
from os import listdir, walk, remove
from pathlib import Path
import subprocess


class WorknoteConan(ConanFile):
    name = "Worknote"
    version = "0.3"
    description = "..."
    url = "https://github.com/luncliff/grpc-worknote"

    settings = "os", "compiler", "build_type", "arch"

    generators = "cmake_find_package"
    requires = [
        "protobuf/3.21.4",
        "grpc/1.48.0",
    ]
    tool_requires = [
        "cmake/3.23.1"
    ]
    default_options = {
        "protobuf:with_zlib": True,
        "protobuf:lite": False,
        "grpc:codegen": True,
        "grpc:cpp_plugin": True,
        "grpc:objective_c_plugin": True,
        "grpc:python_plugin": True,
    }

    def imports(self):
        source_dir = Path(__file__).parent
        self.copy("*.h", src="include",
                  dst=join(source_dir, "externals", "include"))
        self.copy("*.hpp", src="include",
                  dst=join(source_dir, "externals", "include"))
        self.copy("*.inl", src="include",
                  dst=join(source_dir, "externals", "include"))

        if self.settings.os == "Windows":
            self.copy("*.dll", src="bin",
                      dst=join(source_dir, "externals", "bin"))
            self.copy("*.pdb", src="bin",
                      dst=join(source_dir, "externals", "bin"))
            self.copy("*.lib", src="lib",
                      dst=join(source_dir, "externals", "lib"))

        elif self.settings.os == "Macos":
            self.copy("*.dylib", src="lib",
                      dst=join(source_dir, "externals", "lib"))

    # https://docs.conan.io/en/latest/developing_packages/package_layout.html
    def layout(self):
        cmake_layout(self)

    # # https://docs.conan.io/en/latest/reference/build_helpers/cmake.html
    # def build(self):
    #     cmake = CMake(self)
    #     if self.settings.os == "Windows":
    #         cmake.append_vcvars = True
    #     cmake.configure()
    #     cmake.build()

    # def test(self):
    #     cmake = CMake(self)
    #     if self.settings.os == "Windows":
    #         cmake.append_vcvars = True
    #     cmake.test()

    # def package(self):
    #     cmake = CMake(self)
    #     if self.settings.os == "Windows":
    #         cmake.append_vcvars = True
    #     cmake.install()

    # # https://docs.conan.io/en/latest/reference/conanfile/methods.html#package-info
    # def package_info(self):
    #     self.cpp_info.includedirs = ["include"]
    #     self.cpp_info.libdirs = ["lib"]
    #     self.cpp_info.bindirs = ["bin"]
    #     self.cpp_info.defines = []
    #     self.cpp_info.cxxflags = []
    #     if self.options.shared == True:
    #         self.cpp_info.sharedlinkflags = []
    #     self.cpp_info.libs = ["Worknote"]