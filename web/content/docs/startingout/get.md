---
title: Get Dissolve
weight: 1
description: Where to find/get Dissolve
---
{{< label color="blue" icon="fa-clock" text="2 minute read" >}}

## Pre-Built Packages

Dissolve is available in packaged / installable form for several common operating systems from the [packages]({{< ref "packages" >}}) page.  Note that these packages focus primarily on providing the GUI version of the code, which can be used for setting up / visualising simulations, as well as running small-scale simulations such as the [examples]({{< ref "examples" >}}).

For serious simulations, the parallel MPI version is recommended, but which must usually be compiled by hand on the target machine on a case-by-case basis.

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
