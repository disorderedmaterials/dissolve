---
title: Compilation
description: Building Dissolve from scratch
weight: 4
---

{{< tip text="Pre-built binaries for most desktop systems are available in the [packages](https://www.projectdissolve.com/packages) section - don't compile Dissolve if you don't have to!" />}}

Dissolve uses `CMake` in order to provide a consistent build system across Window, OSX, and Linux.

## Prerequisites

### Build Toolchain

Dissolve is completely written in C++ and uses some aspects of the C++17 standard and parallel STL, so a compiler that supports C++17 and PSTL is a must (for `gcc` this means version 9 or above, for example). The CMake build system is used throughout.

You may also consider using the excellent [`Ninja`](https://ninja-build.org/) to speed up your build. If you choose to do so, add `-G Ninja` to the `cmake` commands detailed below, and run `ninja` instead of `make`.

### Common External Libraries

Regardless of which version is being built, several external libraries are always required:

- [`libfmt`](https://github.com/fmtlib/fmt) >= v7, to provide flexible and type-safe output formatting
- [`CLI11`](https://github.com/CLIUtils/CLI11) >= v1.9.1, to provide coherent handling of command line arguments
- [`ANTLR4`](https://www.antlr.org/) >= v4.7.2, required to implement various grammars (e.g. NETA)

Satisfying these dependencies is enough to build the serial version. For the parallel and GUI versions additional libraries are required, detailed in the following sections.

Dissolve uses the [`Conan`](https://conan.io/) package manager to satisfy external dependencies as far as possible. `ANTLR4` is not available via Conan (yet) and so must either be installed from platform-specific repositories or built alongside Dissolve using the relevant configuration options. See the [compilation instructions]({{< ref "compilation#install-antlr4" >}}) for more information.

### MPI (for versions 0.5.X, 0.6.X, and 0.7.X)

Version 0.7.X of Dissolve uses MPI for parallelism, so a suitable MPI implementation providing `mpic++` is required - MPI-based parallelism is enabled by passing `-DPARALLEL:bool=true` to `cmake`. It is *not* recommended to build version 0.8 with MPI enabled, since MPI+threading is only experimental in this version.

### Multithreading (for version 0.8.X onwards)

As of version 0.8 the default is for Dissolve to be built with multithreading enabled - this applies to both the command-line and GUI versions. If you really want a truly serial code, pass `-DMULTI_THREADING:bool=false` to `cmake`.

### GUI

The GUI requires several external graphical libraries and toolkits:

- Qt6 (Widgets, Core, OpenGL) v6.2 or higher (including development libraries/headers)
- FTGL (including development libraries/headers)
- Freetype2 (including development libraries/headers)

To build the GUI, pass `-DGUI:bool=true` to `cmake`.

## Prepare the Build

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

If you have the ANTLR4 tool and/or runtime library installed on your system, but they can't be located by `cmake`, try the `ANTLR_DIR` option:

```
-DANTLR_DIR:path=/here/is/my/ANTLR4
```

If you have downloaded the ANTLR4 Java tool and need to specify it explicitly, use the `ANTLR_EXECUTABLE` option. For example:

```
-DANTLR_EXECUTABLE:path=/my/downloads/antlr-4.8-complete.jar
```

### Full Example

An example`cmake` command using Ninja to build both the command-line and GUI codes, building ANTLR4 as an external project, and locating the ANTLR4 Java tool in the build directory, would look like this:

```
cmake ../ -G Ninja -DGUI:bool=true -DBUILD_ANTLR_RUNTIME:bool=true -DANTLR_EXECUTABLE:path=./antlr-4.13.1-complete.jar
```

## Run the Build

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

#### `ANTLR_DIR`

If the pre-installed ANTLR4 C++ runtime library and headers cannot be found easily by `cmake` (e.g. it is in a custom location) or you need to download and use a copy local to the build, the `ANTLR_DIR` can be used to specify an absolute path to the installation. The specified folder is expected to contain `lib` and `include` directories containing the relevant files.

Example: `-DANTLR_DIR:path=/path/to/antlr-runtime/`

Default: `not set`

#### `BUILD_BENCHMARKS`

In addition to the main build, also build the benchmarks located in the benchmark directory. If this option is specified, the Google benchmark library will be downloaded and compiled by cmake.

Usage: `-DBUILD_BECHMARKS:bool=true`

Default: `false`

#### `BUILD_TESTS`

In addition to the main build, also build tests located in the `tests/` directory.  The tests check individual portions of the dissolve code base for problems, and run dissolve on a large number of known data sets to confirm that there are no error in those calculations.

Example: `-DBUILD_TESTS:bool=true`

Default: `false`

#### `GUI`

Requests that the GUI version of Dissolve be built. This option is mutually exclusive with `-DPARALLEL:bool=true`. Qt5 libraries and development tools must be present in the path, as well as [`FTGL`](http://ftgl.sourceforge.net/docs/html/).

Usage: `-DGUI:bool=true`

Default: `false`

#### `PARALLEL`

Requests that the parallel version of Dissolve be built. This option is mutually exclusive with `-DGUI:bool=true`.  A suitable OpenMPI-compatible MPI compiler must be present in the path.

Usage: `-DPARALLEL:bool=true`

Default: `false`

#### `MULTI_THREADING`

Requests that the multithreaded version of Dissolve be built. Intel thread building blocks (tbb) must be present in the path. By default this option is enabled.

Usage: `-DMULTI_THREADING=ON`

Default: `true`
