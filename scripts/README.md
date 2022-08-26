This directory contains scripts for building on "exotic" platforms, as well as for performing some other useful platform-specific tasks.

## Build Scripts
`slurm-compile-mpi.script` - SLURM script to compile the MPI version of Dissolve (for use on e.g. SCARF).
`slurm-compile-threads.script` - SLURM script to compile the threaded version of Dissolve (for use on e.g. SCARF).
`idaaas-compile-threads.sh` - Bash script to compile the threaded version on IDAaaS (v0.8.X using Qt5 only)

## SCARF Install Scripts
`scarf-add-module` - Bash script to create a module referencing a specific major/minor/patch version.
`scarf-add-module.runscript` - Run script fragment used by `scarf-add-module`.
`scarf-add-module.modulefile` - Module file fragment used by `scarf-add-module`.
