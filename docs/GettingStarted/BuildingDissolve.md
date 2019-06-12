---
title: Building Dissolve
parent: Getting Started
---
# Building Dissolve

Dissolve can be build with either `autotools` or `CMake` - the latter is definitely easier on OSX and Windows systems. For Linux-based systems, choose whichever you feel most comfortable with.

Note that the serial and parallel versions must be build separately. Building the GUI will also build the serial version (since it is trivial to do so).

## Prerequisites

The following must be installed / available on the target system in order to build Dissolve.

### Dissolve (Serial Version)

- g++ or similar
- Bison

### Dissolve (Parallel Version)

- g++ or similar
- mpic++ (e.g. from OpenMPI)
- Bison

### GUI

- g++ or similar with C++11 support
- Bison
- Qt5 (Widgets, Core, OpenGL) v5.6 or higher (including development libraries/headers)
- FTGL (including development libraries/headers)
- Freetype2 (including development libraries/headers)

## Autotools

### Prepare the Build System
As a first step, prepare the build system with the supplied `autogen.sh` script:

```
bob@linux:~> ./autogen.sh
```

### Configure the Build System

Afterwards, you can run `configure` to check for all the necessary prerequisites and prepare the build.

#### Serial Version

```
bob@linux:~> ./configure
```

#### Parallel Version

```
bob@linux:~> ./configure --with-parallel
```

The compiler is assumed to be `mpic++`. If this is not the case, run:

```
bob@linux:~> ./configure --with-parallel=COMPILER
```

...where `COMPILER` is the MPI compiler binary to use.

#### GUI

```
bob@linux:~> ./configure --with-gui
```

As noted above, building the GUI implicitly builds the serial version as well, since the underlying codebase is identical.

It may be necessary to set the locations of the Qt5 resource compiler, meta-object compiler, and user-interface compiler explicitly. The default assumptions are `rcc-qt5`, `moc-qt5`, and `uic-qt5`, but can be changed as follows:

```
bob@linux:~> ./configure --with-gui --with-qtrcc=rcc --with-qtmoc=moc --with-qtuic=uic
```

### Run the Build

Once configured correctly and with no errors, the target binaries can be built simply by running `make`:

```
bob@pc:~> make
```

Running make in parallel is recommended for speed (e.g. `make -j 4`), but the build may need to be restarted part-way through as, depending on the exact order in which the files are built, some dependent files (particularly those output by Bison) may not be available when required.

## CMake

### Prepare the Build

It is best to perform an out-of-tree build with `CMake`, so as a first step create a `build` directory in the root of the source distribution and descend in to it:

```
bob@linux:~> mkdir build && cd build
```

#### Serial Version

```
bob@linux:~> cmake ../
```

#### Parallel Version

```
bob@linux:~> cmake ../ -DPARALLEL:bool=true
```

The compiler is assumed to be `mpic++`.

#### GUI

```
bob@linux:~> cmake ../ -DGUI:bool=true
```

As noted above, building the GUI implicitly builds the serial version as well, since the underlying codebase is identical.

### Run the Build

Once configured correctly and with no errors, the target binaries can be built simply by running `make`:

```
bob@pc:~> make
```
