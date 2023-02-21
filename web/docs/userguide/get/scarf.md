---
title: SCARF
description: Where to find, and how to run, Dissolve on SCARF
weight: 4
---

Dissolve is installed as a module on [SCARF](https://www.scarf.rl.ac.uk), and is built (as of v0.7.3) using the `gcc` compilers.

Of course, you'll need an account on SCARF to use it - register [here](https://www.scarf.rl.ac.uk/registration.html).

## Available Versions

Typically you will want to be using the latest release version of Dissolve which you can request as follows:

```
bob@scarf:~> module load contrib/dissolve
```

If you need to request a specific version for some reason you can list which ones are available with:

```
bob@scarf:~> module avail dissolve

--------------- /apps20/contrib/modulefiles ----------------
contrib/dissolve/0.8.9   contrib/dissolve/0.9.0
```

To source one ready for use in a script run:

```
bob@scarf:~> module load contrib/dissolve/0.9.0
```

## Example Scripts

The following example scripts for SCARF can be used as a basis for your own, or slightly modified and used as-is for the most part, but be aware that at present there are specific scripts for specific versions of Dissolve.

Before using any of these this script, you should:
1. Change `JOBNAME` to a short descriptive name for your job, e.g. `water10k`
2. Request an appropriate number of processors / tasks. How this is done depends on the version of Dissolve being run (see individual scripts for details).
3. Note the job time specified by `-t` is in the format `HH:MM:SS`, so the scripts here will run for six hours before being terminated by the queuing system on SCARF, regardless of whether the simulation has finished or not. Alternatively, you can use the format `DD-HH` to specify the job time in days and hours (e.g. `3-0` for three full days).
4. Set the path to the directory containing your input file for Dissolve - i.e. change `~/path/to/simulation` to `~/work/water/tests/`, for example.
5. Set an appropriate number of iterations for dissolve to run (100 in this example).

Note that, as written here, the associated error and log files written by the system for the run are prepended by the numerical job identifier for the run (e.g. `435003.err`).

### Multithreaded (v0.8.X, v0.9.X)

The multithreaded version of Dissolve operates on a single physical node, with multiple threads running on that node. The run script for SCARF must explicitly request this: a single "task" on a single node which can use 12 CPU cores on that node.

```
#!/bin/bash
#SBATCH -J JOBNAME
#SBATCH -q scarf
#SBATCH -J sles0.4
#SBATCH -q scarf
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=12
#SBATCH -t 06:00:00
#SBATCH -o %j.log
#SBATCH -e %j.err

# Load necessary modules
module load contrib/dissolve

# Change to directory containing simulation
cd ~/path/to/simulation

# Run Dissolve
dissolve input.txt -n 100
```

### Pure MPI (v0.7.X)

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

 This script requests eight processors (`-n 8` in the `#SBATCH` section) all on the same physical node (`--tasks-per-node=8`) - highly recommended for performance reasons. This is a good place to start, but can be increased (or reduced) as necessary.
