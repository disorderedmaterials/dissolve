---
title: Dissolve on SCARF
description: Where to find, and how to run, Dissolve on SCARF
---

The MPI version of Dissolve is installed as a module on [SCARF](https://www.scarf.rl.ac.uk), and is built (as of v0.7.3) using the `gcc` compilers.

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

# Load necessary modules
module load foss
module load contrib/dissolve/0.7.3

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

If you are using a version built with the Intel compilers (<= v0.7.2) then the `module load` lines must be changed to:

```
module load intel/20.0.0
module load intel/mpi/20.0.0
module load gcc/8.1.0
module load contrib/dissolve/0.7.3
```

Some additional environment variables must also be set:

```
export I_MPI_FABRICS="shm:ofi"
export I_MPI_HYDRA_TOPOLIB=ipl
export FI_PROVIDER=verbs
```

### Compiling Dissolve on SCARF

You should never need to compile Dissolve on SCARF yourself, but in case you have a specific version / commit you want to test, or I forget how to do it, here are step-by-step instructions.

[Compilation with gcc (recommended)]({{< ref "scarf-foss" >}})

[Compilation with the Intel compilers]({{< ref "scarf-intel" >}})
