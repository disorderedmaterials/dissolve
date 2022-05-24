---
title: Basic Running
weight: 2
description: Running a simulation
---

Once your [input file]({{< ref "inputfile" >}}) is set up, running Dissolve to execute your simulation is the next step. Depending on exactly what you're doing, there are a few ways of doing so.

## Running a Simulation

### From the GUI

There are pros and cons of running the simulation through the GUI. On the one hand, you can investigate / plot / interrogate the simulation as it is running. On the other, the GUI invokes some overhead from displaying all the information, and so is not as performant as the CLI version. However, as of version 0.8 the GUI is multithreaded, so can be used for production simulations of small to medium systems.

The "Simulation" menu allows simulations to be stopped and started, or the following keyboard shortcuts can be used instead:

|Shortcut|Action|
|:-----:|------|
|`Ctrl-R`|Run the simulation until interrupted.|
|`Esc`|Stop a running simulation. This may not happen immediately since the current iteration will always run to completion.|
|`Ctrl-1`|Perform a single iteration.|
|`Ctrl-5`|Perform five iterations.|
|`Ctrl-F`|Run for a specific number of iterations - a dialog will pop up asking for the number to run.|

The status bar at the bottom of Dissolve's main window will tell you the current status ("Idle", "Running" etc.) and, equally importantly, will show you when errors are detected. If errors are reported, go to the "Messages" tab to get more information on what went wrong.

### From the Command Line

Using the command-line versions of Dissolve, the number of iterations to run must be provided with the [`-n`]({{< ref "cli#-n-n---niterations-n" >}}) option. For instance, to run the simulation defined in `input.txt` for 100 iterations:

```
dissolve -n 100 input.txt
```

Prior to version 0.8 the `dissolve` executable was serial only, but as of 0.8 is multithreaded by default. This means that it will utilise as many cores your machine has to offer. Note that, since the multithreaded versions is built on Intel's Threading Building Blocks you do *not* need to specify how many cores are available for use - the system will give over as much power as it can afford. The multithreaded version on its own can only be run on a single machine - e.g. not across nodes in a cluster, for instance.

To run the hybrid MPI/multithreaded version of Dissolve, or the old MPI code (versions up to and including 0.7.5):

```
mpirun dissolve-mpi -np 8 -n 100 input.txt
```

This uses the ([OpenMPI](https://www.open-mpi.org/)) `mpirun` command to run `dissolve-mpi` across 8 tasks, which may be over different physical nodes.

## Restarting a Simulation

No matter how you run Dissolve, a [restart file]({{< ref "restart" >}}) is periodically written, and which contains the complete state of the simulation. The default is to do so every ten steps, but this can be controlled with the [`-f`]({{< ref "cli#-f-n---frequency-n" >}}) switch. Following completion of the requested number of steps or, if you're unlucky, a crash, the simulation can be restarted using the data contained within the [restart file]({{< ref "restart" >}}).

The restart file is always named after the input file so, if it exists, Dissolve will automatically read it in before continuing the simulation. The GUI offers slightly more flexibility than the command-line version in this regard, and will prompt you for which restart file to load if it finds one or more in the input file directory.
