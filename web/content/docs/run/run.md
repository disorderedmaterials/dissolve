---
title: Basics
weight: 1
description: Performing the simulation
---

Once your [input file]({{< ref "inputfile" >}}) is set up, running Dissolve to execute your simulation is the next step. Depending on exactly what you're doing, there are a few ways of doing so.

### Running a Simulation

#### From the GUI

Running your simulation from the GUI is simply a case of clicking the large "run" button at the top-left of the window (or using the shortcut `Ctrl-R`). This will run the simulation continually until it is stopped.

{{< cimage "" >}}

There are pros and cons of running the simulation through the GUI. On the one hand, you can investigate / plot / interrogate the simulation as it is running. On the other, the GUI is only single-threaded (i.e. serial) so cannot realistically be used for simulations working with  systems containing larger numbers of atoms. However, for running tutorials and smaller production simulations, the GUI is a good place to be.

#### From the Command Line

Using the command-line versions of Dissolve, the number of iterations to run must be provided with the [`-n`]({{< ref "cli#-n-n---niterations-n" >}}) option. For instance, to run the simulation defined in `input.txt` for 100 iterations:

```
dissolve -n 100 input.txt
```

The `dissolve` executable used here is the _serial_ version of the code, and is equivalent to the GUI in that sense. Most of the time, and especially for larger simulations, running Dissolve in parallel from the command line is the recommended approach.

```
mpirun dissolve-mpi -np 8 -n 100 input.txt
```

This uses the ([OpenMPI](https://www.open-mpi.org/)) `mpirun` command to run the parallel `dissolve-mpi` on 8 processors / cores.

### Restarting a Simulation

No matter how you run Dissolve, a [restart file]({{< ref "restart" >}}) is periodically written, and which contains the complete state of the simulation. Following completion of the requested number of steps or, if you're unlucky, a crash, the simulation can be restarted using the data contained within the [restart file]({{< ref "restart" >}}).

The restart file is always named after the input file so, if it exists, Dissolve will automatically read it in before continuing the simulation.
