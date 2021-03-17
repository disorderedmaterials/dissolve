---
title: Compiling Dissolve on SCARF (Intel)
description: How to build Dissolve on SCARF using the Intel compilers
---

Note that [compilation using the gcc/FOSS toolchain]({{< ref "scarf-foss" >}}) is recommended.

## 1. Request Interactive Resources

Don't compile Dissolve on the login nodes of SCARF. Instead, use the queue and request an interactive session:

```
srun -p devel --pty /bin/bash
```

## 2. Load Required Modules

Dissolve needs a compiler with C++17 support, which on SCARF means the Intel Compiler v2020 backed by gcc 8.1.0, and also a relatively recent CMake:

```
module load intel/20.0.0
module load intel/mpi/20.0.0
module load gcc/8.1.0
module load cmake/3.10.2
```

## 3. Install Conan

Conan is not installed on SCARF, so you must install a user-space copy of it in order to get some of Dissolve's development prerequisites:

```
pip3 install --user conan
```

Copy and paste the following into your `.conan/profiles/default` (create this file if it doesn't exist):

```
[settings]
arch=x86_64
arch_build=x86_64
build_type=Release
compiler=intel
compiler.base=gcc
compiler.base.libcxx=libstdc++11
compiler.base.version=8
compiler.version=19
os=Linux
os_build=Linux
```

## 4. Retrieve ANTLR4

Both parts of ANTLR4 - the Java tool and runtime library - must be manually dealt with, but this is not too onerous. The Java tool can be downloaded to your Dissolve `build` directory with `wget`. At time of writing, the current version of ANTLR is 4.8:

```
wget https://www.antlr.org/download/antlr-4.8-complete.jar
```

The runtime library can be built as part of the main Dissolve build (see next step). Download the source zipfile to the `build` directory as well (this is not usually necessary, but there appear to be some network issues when retrieving the source via the external project mechanism):

```
wget https://www.antlr.org/download/antlr4-cpp-runtime-4.8-source.zip
```

We'll use the `BUILD_ANTLR_ZIPFILE` option to specify that we want to use this local archive instead of downloading it.

## 5. Configure and Build

In your Dissolve `build` directory, run:

```
conan install ..
cmake .. -DCMAKE_CXX_FLAGS:string="-std=c++17" -DPARALLEL:bool=true -DBUILD_ANTLR_RUNTIME:bool=true -DANTLR_EXECUTABLE:path=`pwd`/antlr-4.8-complete.jar -DBUILD_ANTLR_ZIPFILE:path=`pwd`/antlr4-cpp-runtime-4.8-source.zip
```

Note that the `CMAKE_CXX_FLAGS` must be explicitly set to force the use of the C++17 standard, as this option is not correctly passed down by `cmake`.

## 6. Wait

And then some. Once complete, the `dissolve-mpi` binary can be found in the `build/bin` directory.
