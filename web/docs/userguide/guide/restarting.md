---
title: Checkpointing / Restarting
description: Checkpointing and restarting simulations
weight: 6
---
## Restarting a Simulation

No matter how you run Dissolve, a restart file is periodically written (unless you explicitly request it not to be), and which contains the complete state of the simulation. The default is to do so every ten steps, but this can be controlled with the [`-f`]({{< ref "cli#-f-n---frequency-n" >}}) switch. Following completion of the requested number of steps or, if you're unlucky, a crash, the simulation can be restarted using the data contained within the restart file.

The restart file is always named after the input file so, if it exists, Dissolve will automatically read it in before continuing the simulation. The GUI offers slightly more flexibility than the command-line version in this regard, and will prompt you for which restart file to load if it finds one or more in the input file directory.



## Input

Dissolve takes a single [main input file]({{< ref "inputfile" >}}) that contains all of the definitions for species, configurations, forcefield parameters, and the actual description of what simulation to perform. Essentially, this file is a blueprint for the entire simulation, but does not contain any information on the current _state_ of the simulation. The current state of the simulation - every calculated property, every coordinate of every atom etc. - is stored in the restart file. If it exists this file is read in immediately after Dissolve has finished parsing the [main input file]({{< ref "inputfile" >}}), and is written out periodically as the simulation progresses. If you remove your restart file (or choose to ignore it with the [`-i` switch]({{< ref "cli#-i---ignore-restart" >}})) then your simulation is effectively reset, and begins again from the start with no memory of anything.

The restart file is named after the input file, with the suffix `.restart` appended - e.g. if the input file is `water-3000.txt` the restart file for the simulation will be `water-3000.txt.restart`. Dissolve also keeps a second, backup restart file with the suffix `.restart.bak`, containing the previous saved state of the simulation.


## Workflow

Once the input file(s) have been successfully loaded, Dissolve enters its main loop, beginning from the last iteration number stored in the restart file, if present. The main loop steps through all defined [layers]({{< ref "inputfile#layer" >}}) of [modules]({{< ref "modules" >}}) and executes them sequentially, at the frequencies defined. This is the core of Dissolve'contained in that tell Dissolve "what to do".

From the command-line, a useful number of iterations to run can be specified with the [`-n`]({{< ref "cli#-i-n--iterations-n" >}}) [command-line option]({{< ref "cli" >}}).

## Output

Dissolve writes its restart file every 10 iterations of the main loop, unless specified otherwise. For safety, a backup of the previous restart file is kept in `.restart.prev`, in case something bad should happen. It is important to note that no other output files are written. Individual modules may provide options to save specific data or calculated quantities as the simulation progresses, but the Dissolve GUI should be the principal tool for observing progress in a simulation.