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
        "ms-gsl/3.1.0",
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
        # "grpc:python_plugin": True,
    }

    def imports(self):
        source_dir = Path(__file__).parent
        self.copy("*.h", src="include",
                  dst=join(source_dir, "externals", "include"))
        self.copy("*.hpp", src="include",
                  dst=join(source_dir, "externals", "include"))
        self.copy("*.inl", src="include",
                  dst=join(source_dir, "externals", "include"))
        self.copy("*.proto", src="include",
                  dst=join(source_dir, "externals", "include"))

        if self.settings.os == "Windows":
            self.copy("*.dll", src="bin",
                      dst=join(source_dir, "externals", "bin"))
            self.copy("*.pdb", src="bin",
                      dst=join(source_dir, "externals", "bin"))
            self.copy("*.lib", src="lib",
                      dst=join(source_dir, "externals", "lib"))

        elif self.settings.os == "Macos":
            self.copy("*.a", src="lib",
                      dst=join(source_dir, "externals", "lib"))
            self.copy("*.dylib", src="lib",
                      dst=join(source_dir, "externals", "lib"))

    # https://docs.conan.io/en/latest/developing_packages/package_layout.html
    def layout(self):
        cmake_layout(self)
