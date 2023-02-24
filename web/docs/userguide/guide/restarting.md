---
title: Checkpointing / Restarting
description: Checkpointing and restarting simulations
weight: 6
---

## The Restart File

While the [input file]({{< ref "inputfile" >}})  contains a blueprint of the simulation you want to do, the _state_ of the simulation is contained in the restart file. Your simulation will generate a fair amount of data such as the pair distribution functions, structure factors, calculated quantities and, perhaps most importantly of all, the positions of all atoms in the configuration box(es). No matter how you run Dissolve, a restart file is periodically written and which contains all of this data and represents the complete state of the simulation. The default is to do so every ten steps, but this can be controlled with the [`-f`]({{< ref "cli#-f-n---frequency-n" >}}) switch if running from the CLI, or via the _Simulation_ menu in the GUI.

{{< tip text="You can explicitly turn off writing of the restart file if you really want to - from the CLI pass the [`-x`](#-x---no-restart-file) option, and from the GUI set the write frequency to zero via the _Simulation_ menu." />}}

### Naming Convention

A restart file can be called whatever you want, but Dissolve will *always* write a new restart file based on the name of the current input file, and with a `.restart.` suffix. So, if your input file is `water-3000.txt` the restart file for the simulation will be `water-3000.txt.restart`.

Dissolve also keeps a second, backup restart file with the suffix `.restart.bak`, containing the previous saved state of the simulation. This is to protect against corrupt primary restart files arising from a crash, or termination of the code whilst the restart file is being written.

As alluded to, once Dissolve has finished writing the restart file you can copy / move it and call it whatever you like.

## Using Restart Files

Dissolve will look for a restart file named according to the convention above (e.g. `input.txt.restart`) once it has finished processing and checking the input file. If this file exists it will then be read in and the simulation will be back in the state defined in the file. From the CLI this behaviour can be overridden with the [`-i`](#-i---ignore-restart) switch, forcing Dissolve to ignore this file if it exists - then your simulation is effectively reset, and begins again from the start with no memory of anything.

You can also specify a different restart file to process with the [`--restart`](#--restart) option, but note that a new restart file will still be written to the conventional name (`input.txt.restart`).

When running the GUI Dissolve will look for _any_ `.restart` files in the same directory as the input file, prompting you accordingly on which one you wish to load.

## Workflow

Once the input file(s) have been successfully loaded, Dissolve enters its main loop, beginning from the last iteration number stored in the restart file, if present. The main loop steps through all defined [layers]({{< ref "inputfile#layer" >}}) of [modules]({{< ref "modules" >}}) and executes them sequentially, at the frequencies defined. This is the core of Dissolve'contained in that tell Dissolve "what to do".

From the command-line, a useful number of iterations to run can be specified with the [`-n`]({{< ref "cli#-i-n--iterations-n" >}}) [command-line option]({{< ref "cli" >}}).
