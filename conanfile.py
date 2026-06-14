from conan import ConanFile
from conan.tools.cmake import cmake_layout
from conan.tools.files import copy


class DissolveRecipe(ConanFile):
    name = "Dissolve"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"
    deployers = "direct_deploy"
    options = {
        "msvc_dev": [True, False],
        "tests": [True, False],
        "benchmarks": [True, False],
    }
    default_options = {
        "msvc_dev": False,
        "tests": True,
        "benchmarks": False,
    }
    def configure(self):
        self.options["puxixml"].header_only = False
        self.options["antlr4-cppruntime"].shared = True
        self.options["hwloc"].shared = True

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def requirements(self):
        self.requires("cli11/2.5.0")
        self.requires("pugixml/1.15")
        self.requires("onetbb/2021.10.0")
        self.requires("onedpl/2022.3.0")
        self.requires("toml11/3.7.0")
        self.requires("antlr4-cppruntime/4.13.1")
        self.requires("gsl/2.7.1")

        if self.options.tests:
            self.requires("gtest/1.17.0")
        if self.options.benchmarks:
            self.requires("benchmark/1.8.4")

    def build_requirements(self):
        self.tool_requires("cmake/3.27.9")

    def layout(self):
        cmake_layout(self)

    def package(self):
        copy(
            self,
            pattern="*.dll",
            src=self.build_folder,
            dst=os.path.join(self.package_folder, "bin"),
            keep_path=False,
        )
        copy(
            self,
            pattern="*.dylib",
            src=self.build_folder,
            dst=os.path.join(self.package_folder, "lib"),
            keep_path=False,
        )
        copy(
            self,
            pattern="*.so",
            src=self.build_folder,
            dst=os.path.join(self.package_folder, "lib"),
            keep_path=False,
        )
