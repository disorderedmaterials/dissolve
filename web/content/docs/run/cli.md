---
title: CLI Options
description: Dissolve's command-line options
---

Dissolve takes a small number of command-line options in order to allow some fine control over what it's doing, and also allows for some additional functionality (e.g. input file checking) that doesn't involve running a simulation.

Most options are common to all three flavours of Dissolve (serial, parallel, and the GUI) but some are specific to one in particular. These are listed in separate tables below.

Note that an input file must be provided for the serial and parallel codes - it is optional for the GUI.

### Basic Control

The following options are recognised by the serial and parallel codes, as well as the GUI.

#### `-n <n>`, `--niterations <n>`
Run Dissolve for the specified number of iterations, and then quit. For the GUI version, the specified number of iterations will be run, and then the GUI launched. The default number of iterations if the `-n` flag is not specified is zero - in this case Dissolve will load the input file (and any associated restart file) and then quit, essentially corresponding to a sanity check of the input files.

#### `-q`, `--quiet`
Don't print any output to the console whatsoever. Output files such as the restart file, heartbeat, and any data output requested in individual modules will still be written. Use the [`-x`](#-x---no-files) flag to prevent the restart and heartbeat files being written.

#### `-v`, `--verbose`
Print lots more output, mostly useful for debugging

### Input Files

#### `-i`, `--ignore-restart`
Ignore the restart file (i.e. don't read it in) if it exists. The name of the expected restart file is the input file name suffixed with `.restart`.

#### `-w <filename>`, `--write-input <filename>`
Write the specified input file out to the file specified. Again, mostly useful for debugging.

Not available in the GUI code.

### Output Files

#### `-f <n>`, `--frequency <n>`
Specify the frequency (in terms of main loop iterations) that the restart file should be written at. The default value is 10.

#### `--restart <filename>`
Read restart file data from the file specified, rather than the default. Note that this only affects the initial target for reading the restart file - restart data written by the simulation will still be saved to the default restart file name.

#### `-x`, `--no-files`
Inhibit writing of the restart and heartbeat files. Data exported from individual modules will still be written.

### GUI Options

The following options are specific to the GUI.

#### `-I`, `--ignore-state`
Ignore the GUI state file (i.e. don't read it in) if it exists. The name of the expected state file is the input file name suffixed with `.state`.

### Parallel Version Options

The following options are specific to the parallel version of the code.

#### `-a`, `--all`
Write output from all parallel processes, rather than just the master process. Each line of output will be prefixed by the relevant process ID.

#### `--redirect <basename>`
Redirect all output from `stdout` to individual files, one for each process. The provided `basename` is suffixed by the relevant process ID.
