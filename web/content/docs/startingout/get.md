---
title: Get Dissolve
weight: 1
description: Where to find/get Dissolve
---
{{< label color="blue" icon="fa-clock" text="2 minute read" >}}

## The Various Flavours of Dissolve

There are two principal "flavours" of Dissolve:

- A command-line version, suitable for running from the command line / console, potentially on a cluster or HPC resource
- A GUI version, which can be used to set-up simulations, visualise results, and also run simulations

Both the GUI and command-line versions make use of multi-threading for parallel computation in order to be as efficient as possible.

## Parallelism

Prior to version 0.8 Dissolve utilised pure MPI (Message Passing Interface) to enable parallel computation. In version 0.8 the only parallel option available for general use is the multithreaded code - this means that code versions 0.8.X can only be run on a single machine / node, as opposed to the MPI version which could run across multiple machines / nodes. MPI use has not been completely removed, however, with the intention for version 0.9 to make use of combined MPI / multithreading. See the [top-level roadmap]({{< ref "roadmap" >}}) for more details.

## Pre-Built Packages

Dissolve is available in packaged / installable form for several common operating systems from the [packages]({{< ref "packages" >}}) page.  Note that these packages focus primarily on providing the GUI version of the code, which can be used for setting up / visualising simulations, as well as running smaller-scale simulations such as the [examples]({{< ref "examples" >}}).

### Windows 10

Dissolve comes as a full installer as well as a plain zip archive (if you don't want to install it, or don't have the privileges to do so). Note that Dissolve is built using Visual Studio, and so you may need to install the [Visual C++ runtime libraries](https://visualstudio.microsoft.com/downloads/).

### Mac OSX

A disk image is available for recent versions of OSX.

### Linux

Dissolve is packaged in an AppImage, which should be suitable for most distributions. Note that the AppImage must be made executable in order to use it! We do not currently provide rpm or deb packages.

* * *

## Compilation

If a suitable package for your system is not available, or you need to compile it on your local cluster, [full instructions are available]({{< ref "compilation" >}}).

* * *

## HPC / Cluster Applications

### SCARF

If you have access to the [SCARF cluster](https://www.scarf.rl.ac.uk/) then multiple versions of Dissolve are already pre-installed as modules. More information can be found in [Dissolve on SCARF]({{< ref "scarf" >}}).

### Others

For other HPC systems such as your local University cluster, [compiling Dissolve from source]({{< ref "compilation" >}}) is currently the only option.

Singularity containers are a work in progress!
