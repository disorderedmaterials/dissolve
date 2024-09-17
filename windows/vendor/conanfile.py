from conan import ConanFile
import json
import os
import urllib
from pathlib import Path

BASE='base'
GUI='gui'
TESTS='tests'
ALL='all'

class Dissolve(ConanFile):
    name="Dissolve"
    version="1.5.1"
    settings="os", "compiler", "build_type", "arch"
    generators="PremakeDeps"
    
    options = { GUI: [True, False], TESTS: [True, False], ALL: [True, False] }
    default_options = { GUI: False, TESTS: False, ALL: False }

    def requirements(self):
        with open(self.packages(), 'r') as fp:
            packages = json.load(fp)

        config = self.configuration
        
        for pkg_dict in packages:
            if not (pkg_dict["name"] or pkg_dict["version"]):
                raise ValueError(f"Invalid package data: {pkg_dict}")
            
            pkg = self.to_conan_name(pkg_dict['name'], pkg_dict['version'])
            
            if (ALL in config) or (set(pkg_dict["libs"]) & config):
                self.requires(pkg)

    @property
    def configuration(self) -> set:
        """Set of configurations specifying which libraries to install.
        For example: {BASE, GUI, TESTS} configures conan to install packages for (GUI) Dissolve with tests.

        Returns:
            set: Configured library names as set
        """
        selected = {BASE}

        if self.options.gui:
            selected.add(GUI)
        if self.options.tests:
            selected.add(TESTS)
        if self.options.all:
            selected.add(ALL)
        
        return selected
    
    @staticmethod
    def to_conan_name(name: str, version: str) -> str:
        """Properly format package name and version into conan name

        Args:
            name (str): The name of the package
            version (str): The required version

        Returns:
            str: Conan package as {name}/{version}
        """
        return f"{name}/{version}"
    
    @staticmethod
    def packages(filename: str="package-list.json") -> Path:
        """Return safe path to packages JSON

        Args:
            filename (str, optional): The JSON containing a list of conan packages.
            Defaults to "package-list.json".

        Returns:
            Path: Packages JSON safe path
        """
        path_str = os.path.join(os.path.dirname(os.path.abspath(__file__)), filename)
        path = Path(urllib.parse.urlparse(path_str).path)
        
        assert path.is_file(), f"File not found: {path}"
        
        return path