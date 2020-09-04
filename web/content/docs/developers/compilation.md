---
title: Compilation
description: Compilation instructions
---

Dissolve uses `CMake` in order to provide a consistent build system across Window, OSX, and Linux.

Note that the serial and parallel versions must be built separately. Building the GUI will also build the serial version (since it is trivial to do so).

## Prerequisites

The following must be installed / available on the target system in order to build Dissolve.

### Dissolve (Serial Version)

- g++ or similar with C++17 support
- Bison

### Dissolve (Parallel Version)

- g++ or similar with C++17 support
- mpic++ (e.g. from OpenMPI)
- Bison

### GUI

- g++ or similar with C++17 support
- Bison
- Qt5 (Widgets, Core, OpenGL) v5.6 or higher (including development libraries/headers)
- FTGL (including development libraries/headers)
- Freetype2 (including development libraries/headers)

## Prepare the Build

It is best to perform an out-of-tree build with `CMake`, so as a first step create a `build` directory in the root of the source distribution and descend in to it:

```
bob@linux:~> mkdir build && cd build
```

### Serial Version

```
bob@linux:~> cmake ../
```

### Parallel Version

```
bob@linux:~> cmake ../ -DPARALLEL:bool=true
```

The compiler is assumed to be `mpic++`.

### GUI

```
bob@linux:~> cmake ../ -DGUI:bool=true
```

As noted above, building the GUI implicitly builds the serial version as well, since the underlying codebase is identical.

## Run the Build

Once configured correctly and with no errors, the target binaries can be built simply by running `make`:

```
bob@pc:~> make
```

As an alternative to `make`, one might consider using the excellent [`Ninja`](https://ninja-build.org/).
