---
title: Available Flavours
description: The various flavours of Dissolve
weight: 1
---

There are two principal "flavours" of Dissolve:

- A command-line version, suitable for running from the command line / console, potentially on a cluster or HPC resource
- A GUI version, which can be used to set-up simulations, visualise results, and also run simulations

Both the GUI and command-line versions make use of multi-threading for parallel computation in order to be as efficient as possible.

## Parallelism

### Version 0.5.X, 0.6.X, and 0.7.X

These versions of Dissolve use pure MPI (the Message Passing Interface) to achieve parallelism, and employed a replicated data strategy whereby each process / core has knowledge of the entire simulation. While this approach makes it relatively easy to achieve reasonable parallelism, it is very memory intensive.

These versions of Dissolve can be run across multiple physical nodes.

### Version 0.8

In version 0.8 the only parallel option available for general use is the multithreaded code - this means that code versions 0.8.X can only be run on a single machine / node, as opposed to the MPI version which could run across multiple machines / nodes. MPI use has not been completely removed, however, with the intention for version 0.9 to make use of combined MPI / multithreading. See the [top-level roadmap]({{< ref "roadmap" >}}) for more details.

It is not recommended or supported to run this version of Dissolve across multiple physical nodes.
