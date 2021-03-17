---
title: Input, Output, and Workflow
description: An overview of key files and basic operation
---
{{< label color="blue" icon="fa-clock" text="5 minute read" >}}

## Input

Dissolve takes a single [main input file]({{< ref "inputfile" >}}) that contains all of the definitions for species, configurations, forcefield parameters, and the actual description of what simulation to perform. Essentially, this file is a blueprint for the entire simulation, but does not contain any information on the current _state_ of the simulation. The current state of the simulation - every calculated property, every coordinate of every atom etc. - is stored in the [restart file]({{< ref "restart" >}}). The [restart file]({{< ref "restart" >}}), if it exists, is read in immediately after Dissolve has finished parsing the [main input file]({{< ref "inputfile" >}}), and is written to periodically as the simulation progresses. If you remove your restart file (or choose to ignore it with the [`-i` switch]({{< ref "cli#-i---ignore-restart" >}})) then your simulation is effectively reset, and begins again from the start with no memory of anything.

The restart file is named after the input file, with the suffix `.restart` appended - e.g. if the input file is `water-3000.txt` the restart file for the simulation will be `water-3000.txt.restart`. Dissolve also saves a second, backup restart file with the suffix `.restart.bak`, containing the previous saved state of the simulation.

### External Data

Certain modules may require external data - this data does not have to be stored in the same directory as the main input file, since Dissolve uses relative paths for any referenced external files.

* * *

## Workflow

### Main Loop

Once the input file(s) have been successfully loaded, Dissolve enters its main loop (beginning from the last iteration number stored in the restart file, if present). The main loop consists of a sequence of user-defined steps that tell Dissolve "what to do".

### Iterations

Run from the command line, Dissolve will perform five iterations and then exit unless told otherwise. A useful number of iterations to run can be specified with the `-n` switch (see the full list of [command-line options]({{< ref "cli" >}}) for more details.

* * *

## Output

Dissolve writes its [restart file]({{< ref "restart" >}}) every 10 iterations of the main loop, unless specified otherwise. For safety, a backup of the previous restart file is kept in `.restart.prev`, in case something bad should happen. It is important to note that no other output files are written. Individual modules may provide options to save specific data or calculated quantities as the simulation progresses, but the Dissolve GUI should be the principal tool for observing progress in a simulation.

Predicted timing information for the current iteration of the simulation is written to the `.beat` file. This [heartbeat file]({{< ref "heartbeat" >}}) is used by the GUI to allow simple assessment of a simulation's progress when monitoring it remotely.

When running the Dissolve GUI you will also see a `.state` file appear. This contains GUI-specific information such as rendering styles, axis limits, and information on any defined workspaces, and is used to restore the GUI to the state you left it in when you last closed the input file. It is automatically loaded when opening an input file, if it exists, but is not required (i.e. if it gets deleted for some reason the input file will still open correctly).
