from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout
from conan.tools.build import check_min_cppstd

import os


class RESPONDRecipe(ConanFile):
    name = "respond"
    version = "2.0.0"

    options = {
        "shared": [True, False],
        "with_api": [True, False]
    }

    default_options = {
        "shared": False,
        "with_api": False
    }

    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"

    def requirements(self):
        self.requires("eigen/[>=3.4.0]")
        self.requires("spdlog/[>=1.11.0]")
        if self.options.with_api:
            self.requires("crowcpp-crow/1.0+5")
        self.test_requires("gtest/[>=1.13.0]")

    def validate(self):
        check_min_cppstd(self, "17")

    def build_requirements(self):
        self.tool_requires("cmake/[>=3.23]")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        if self.options.with_api:
            tc.variables["WITH_API"] = True
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        if not self.conf.get("tools.build:skip_test", default=False):
            test_folder = os.path.join(str(self.settings.build_type), "tests")
            self.run(os.path.join(test_folder, "respondTest"))

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["RESPOND"]
