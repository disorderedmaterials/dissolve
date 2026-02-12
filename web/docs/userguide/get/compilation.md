---
title: Compilation
description: Building Dissolve from scratch
weight: 4
---

{{< tip text="Pre-built binaries for most desktop systems are available in the [packages](https://www.projectdissolve.com/packages) section - don't compile Dissolve if you don't have to!" />}}

Dissolve uses `CMake` in order to provide a consistent build system across Window, OSX, and Linux.

## General Requirements

### Build Toolchain

Dissolve is completely written in C++ and uses some aspects of the C++20 standard and parallel STL, so a compiler that supports C++20 and PSTL is a must (for `gcc` this means version 9 or above, for example). The CMake build system is used throughout.

You may also consider using the excellent [`Ninja`](https://ninja-build.org/) to speed up your build. If you choose to do so, add `-G Ninja` to the `cmake` commands detailed below, and run `ninja` instead of `make`.

### External Dependencies

Dissolve uses the [`Conan`](https://conan.io/) package manager to satisfy external dependencies as far as possible. The main exceptions to this are the `ANTLR4` Java binary which must be obtained manually and the `Qt6` libraries.

## Windows / MSVC

For Windows users who have Visual Studio there is a PowerShell setup script `develop.ps1` which takes care of most of the tedious setup and prerequisite installation, creating a full Visual Studio environment for the build. This is the recommended way of getting up and running.

## Linux / NixOS

Dissolve provides a `nix` flake for building on Linux systems.

## Linux / Ubuntu 24

To install the necessary system tools and libs on Ubuntu 24, use the following commands:

```
pipx install conan==1.*
sudo apt install cmake ninja-build default-jre libftgl-dev qt6-base-dev qt6-declarative-dev qt6-quick3d-dev qt6-3d-dev libqt6svg6 libtbb12 qt6-shadertools-dev libxkbcommon-dev
wget https://www.antlr.org/download/antlr-4.13.1-complete.jar -Oantlr-4.13.1-complete.jar
```

## General Compilation Notes

### Initialise Submodules

From the top-level directory initialise internal submodules that Dissolve depends on with:

```
git submodule update --init --recursive
```

### Prepare the Build

It is best to perform an out-of-tree build, so as a first step create a `build` directory in the root of the source distribution and descend in to it:

```
mkdir build && cd build
```

### Install Basic Dependencies (conan)

First, install the `conan` Python module if you don't already have it:

```
pip3 install conan
```

If you don't have admin privileges, a user-space install will work):

```
pip3 install --user conan
```

Then simply run `conan` in the `build` directory as follows:

```
conan install ..
```

This will download and locally install necessary external packages locally to the Dissolve build.

### Install ANTLR4

ANTLR4 comprises two distinct parts - a generator tool, written in Java, used at build time to generate code implementing various parser/lexer structures, and a runtime library. The Java component is commonly-available on all operating systems, either as a vendor-provided package or as a straight download from the [ANTLR website](https://www.antlr.org). The runtime library is available for most platforms via conan, and is installed automatically in the previous step.

If you have the ANTLR4 tool and/or runtime library installed on your system, but they can't be located by `cmake`, try the `ANTLR_EXECUTABLE` option:

```
-ANTLR_EXECUTABLE:path=/here/is/my/ANTLR4
```

If you have downloaded the ANTLR4 Java tool and need to specify it explicitly, use the `ANTLR_EXECUTABLE` option. For example:

```
-DANTLR_EXECUTABLE:path=/my/downloads/antlr-4.8-complete.jar
```

You also might have to specify the location of your Java installation if it can't be located by `cmake`:

```
-DJava_JAVA_EXECUTABLE:path=/here/is/my/java
```

### Full CMake Command

An example`cmake` command using Ninja to build both the command-line and GUI codes, building ANTLR4 as an external project, and locating the ANTLR4 Java tool in the build directory, would look like this:

```
cmake ../ -G Ninja -DGUI:bool=true -DBUILD_ANTLR_RUNTIME:bool=true -DANTLR_EXECUTABLE:path=./antlr-4.13.1-complete.jar -DJava_JAVA_EXECUTABLE:path=/here/is/my/java
```

### Run the Build

Once configured correctly and with no errors, the target binary can be built simply by running `make`:

```
make
```

...or, if you installed `ninja` and ran `cmake` with `-G Ninja`:

```
ninja
```

## Configuration Option Reference

The full list of additional `cmake` configuration options for Dissolve are as follows:

#### `ANTLR_EXECUTABLE`

If the ANTLR4 Java tool cannot be found easily by `cmake` (e.g. it is in a custom location) or you need to download and use a copy local to the build, the `ANTLR_EXECUTABLE` can be used to specify an absolute path to the `jar` file.

Example: `-DANTLR_EXECUTABLE:path=/path/to/antlr-4.8-complete.jar`

Default: `not set`

#### `Java_JAVA_EXECUTABLE`

If your Java installation cannot be found by `cmake` the `Java_JAVA_EXECUTABLE` can be used to specify the absolute path to the `java` executable.

Example: `-DANTLR_EXECUTABLE:path=/path/to/java`

Default: `not set`

#### `BUILD_BENCHMARKS`

In addition to the main build, also build the benchmarks located in the benchmark directory. If this option is specified, the Google benchmark library will be downloaded and compiled by cmake.

Usage: `-DBUILD_BECHMARKS:bool=true`

Default: `false`

#### `BUILD_TESTS`

In addition to the main build, also build tests located in the `tests/` directory.  The tests check individual portions of the code for problems, and run Dissolve on a number of known data sets to confirm that there are no errors in those calculations.

Example: `-DBUILD_TESTS:bool=true`

Default: `false`

#### `GUI`

Requests that the GUI version of Dissolve be built. Qt5 libraries and development tools must be present in the path, as well as [`FTGL`](http://ftgl.sourceforge.net/docs/html/).

Usage: `-DGUI:bool=true`

Default: `false`

#### `MULTI_THREADING`

Requests that the multithreaded version of Dissolve be built. Intel thread building blocks (tbb) must be present in the path. By default this option is enabled.

Usage: `-DMULTI_THREADING=ON`

Default: `true`
