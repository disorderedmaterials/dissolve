---
title: Dissolve on SCARF
description: Where to find, and how to run, Dissolve on SCARF
---

The MPI version of Dissolve is installed as a module on [SCARF](https://www.scarf.rl.ac.uk), and is built using the Intel compiler / MPI framework and gcc 8.1.0.

Of course, you'll need an account on SCARF - register [here](https://www.scarf.rl.ac.uk/registration.html).

### Available Versions

To see the available versions, run:

```
bob@scarf:~> module avail dissolve

------------------- /apps/modulefiles --------------------
   contrib/dissolve/0.6.1
   contrib/dissolve/latest

--------------- /apps/contrib/modulefiles ----------------
   dissolve/0.6.1
   dissolve/latest
```

To source one ready for use run:

```
bob@scarf:~> module load contrib/dissolve/1.1.0
```

See the example script below for a full working example. Note that the `latest` version is mostly for testing and is quite volatile. Only use this if you know what you're doing, want to experiment with a bleeding-edge version, or have been told that this is what you should do!

### Example Script

The following example script for SCARF can be used as a basis for your own, or slightly modified and used as-is for the most part. See the notes below for instructions on what you should change for your own runs.

```
#!/bin/bash
#SBATCH -J JOBNAME
#SBATCH -q scarf
#SBATCH -C scarf18
#SBATCH -n 8
#SBATCH --tasks-per-node=8
#SBATCH -t 06:00:00
#SBATCH -o %j.log
#SBATCH -e %j.err

# Load necessary modules and set-up some environment variables
module load intel/20.0.0
module load intel/mpi/20.0.0
module load gcc/8.1.0
export I_MPI_FABRICS="shm:ofi"
export FI_PROVIDER=verbs

# Load Dissolve
module load contrib/dissolve/0.6.1

# Change to directory containing simulation
cd ~/path/to/simulation

# Run Dissolve
mpirun dissolve-mpi input.txt -n 100
```

Before using this script, you should:
1. Change `JOBNAME` to a short descriptive name for your job, e.g. `water10k`
2. Request an appropriate number of processors. As written, the script requests eight processors (`-n 8` in the `#SBATCH` section) all on the same physical node (`--tasks-per-node=8`) - highly recommended for performance reasons. This is a good place to start, but can be increased (or reduced) as necessary.
3. Note the job time specified by `-t` is in the format `HH:MM:SS`, so the script above will run for six hours before being terminated by the queuing system on SCARF, regardless of whether the simulation has finished or not.
4. Set the path to the directory containing your input file for Dissolve - i.e. change `~/path/to/simulation` to `~/work/water/tests/`, for example.
5. Set an appropriate number of iterations for dissolve to run (100 in this example).

Note that, as written here, the associated error and log files written by the system for the run are prepended by the numerical job identifier for the run (e.g. `435003.err`).

### Compiling Dissolve on SCARF

You should never need to compile Dissolve on SCARF yourself, but in case you have a specific version / commit you want to test, or I forget how to do it, here are step-by-step instructions.

#### 1. Request Interactive Resources

Don't compile Dissolve on the login nodes of SCARF. Instead, use the queue and request an interactive session:

```
srun -p devel --pty /bin/bash
```

#### 2. Load Required Modules

Dissolve needs a compiler with C++17 support, which on SCARF means the Intel Compiler v2020 backed by gcc 8.1.0, and also a relatively recent CMake:

```
module load intel/20.0.0
module load intel/mpi/20.0.0
module load gcc/8.1.0
module load cmake/3.10.2
```

#### 3. Install Conan

Conan is not installed on SCARF, so you must install a user-space copy of it in order to get some of Dissolve's development prerequisites:

```
pip3 install --user conan
```

#### 4. Retrieve ANTLR4

Both parts of ANTLR4 - the Java tool and runtime library - must be manually dealt with, but this is not too onerous. The Java tool can be downloaded to your Dissolve `build` directory with `wget`. At time of writing, the current version of ANTLR is 4.8:

```
wget https://www.antlr.org/download/antlr-4.8-complete.jar
```

The runtime library can be built as part of the main Dissolve build (see next step). Download the source zipfile to the `build` directory as well (this is not usually necessary, but there appear to be some network issues when retrieving the source via the external project mechanism):

```
wget https://www.antlr.org/download/antlr4-cpp-runtime-4.8-source.zip
```

We'll use the `BUILD_ANTLR_ZIPFILE` option to specify that we want to use this local archive instead of downloading it.

#### 5. Configure and Build

In your Dissolve `build` directory, run:

```
conan install ..
cmake .. -DCMAKE_CXX_FLAGS:string="-std=c++17" -DPARALLEL:bool=true -DBUILD_ANTLR_RUNTIME:bool=true -DANTLR_EXECUTABLE:path=`pwd`/antlr-4.8-complete.jar -DBUILD_ANTLR_ZIPFILE:path=`pwd`/antlr4-cpp-runtime-4.8-source.zip
```

Note that the `CMAKE_CXX_FLAGS` must be explicitly set to force the use of the C++17 standard, as this option is not correctly passed down by `cmake`.

#### 6. Wait

And then some. Once complete, the `dissolve-mpi` binary can be found in the `build/bin` directory.
