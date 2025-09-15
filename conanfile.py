from conan import ConanFile
from conan.tools.cmake import cmake_layout
from conan.tools.files import copy


class DissolveRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def configure(self):
        self.options["puxixml"].header_only=False
        self.options["antlr4-cppruntime"].shared=True

    def requirements(self):
        self.requires("cli11/1.9.1")
        self.requires("pugixml/1.15")
        self.requires("onetbb/2021.10.0")
        self.requires("onedpl/2022.3.0")
        self.requires("toml11/3.7.0")
        self.requires("antlr4-cppruntime/4.13.1")
        self.requires("gsl/2.7.1")

        if self.settings.os == "Linux":
            self.requires("gtest/1.17.0")
            self.requires("benchmark/1.8.4")

    def build_requirements(self):
        self.tool_requires("cmake/3.27.9")

    def layout(self):
        cmake_layout(self)
