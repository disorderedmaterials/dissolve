---
title: 5. The CLI
description: Dissolve from the Command Line
weight: 5
---

## Basic Usage

Dissolve expects one mandatory argument - the name of the input file to load:

```
dissolve input.txt
```

Note that Dissolve will not actually start running a simulation with just this information - instead, it will check the validity of both the input file and the proposed simulation and then exit.

Typically, then, you will also need to provide the number of iterations to run with the [`-n`]({{< ref "cli#-n-n---niterations-n" >}}) option. For instance, to run the simulation defined in `input.txt` for 100 iterations:

```
dissolve -n 100 input.txt
```

## Parallelism

Dissolve is multithreaded by default and will utilise as many cores your machine has to offer. Note that, since the multithreaded versions is built on Intel's Threading Building Blocks you don't need to specify how many cores are available for use - the system will give over as much power as it can afford.

The multithreaded version can only be run on a single machine - i.e. not across nodes in a cluster, for instance.

## CLI Option Reference

Dissolve takes a small number of command-line options in order to allow some fine control over what it's doing, and also allows for some additional functionality (e.g. input file checking) that doesn't involve running a simulation.

Most options are common to all flavours of Dissolve (serial, multithreaded, MPI, and the GUI) but some are specific to one in particular. These are listed in separate tables below.

Note that an input file does not need to be provided to the GUI version but is required for all other versions.

### Basic Control

The following options are recognised by the serial and multithreaded codes, as well as the GUI.

#### `-n <n>`, `--niterations <n>`
Run Dissolve for the specified number of iterations, and then quit. For the GUI version, the specified number of iterations will be run, and then the GUI launched. The default number of iterations if the `-n` flag is not specified is zero - in this case Dissolve will load the input file (and any associated restart file) and then quit, essentially corresponding to a sanity check of the input files.

#### `--seed <n>`
Give a specific random seed with which to initialise the random number generator.

#### `-q`, `--quiet`
Don't print any output to the console whatsoever. Output files such as the restart file and any data output requested in individual modules will still be written. Use the [`-x`](#-x---no-restart-file) flag to explicitly disable writing of the restart file.

#### `-v`, `--verbose`
Print lots more output, mostly useful for debugging

### Input Files

#### `-i`, `--ignore-restart`
Ignore the restart file (i.e. don't read it in) if it exists. The name of the expected restart file is the input file name suffixed with `.restart`.

#### `--restart <filename>`
Read restart file data from the file specified, rather than the default. Note that this only affects the initial target for reading the restart file - restart data written by the simulation will still be saved to the default restart file name.

#### `-w <filename>`, `--write-input <filename>`
Write the specified input file out to the file specified. Again, mostly useful for debugging.

Not available in the GUI code.

### Output Files

#### `-f <n>`, `--frequency <n>`
Specify the frequency (in terms of main loop iterations) that the restart file should be written at. The default value is 10.

#### `-x`, `--no-restart-file`
Prevent writing of the restart file completely. Data exported from individual modules will still be written.
