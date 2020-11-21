---
title: Get Dissolve
weight: 1
description: Where to find/get Dissolve
---
{{< label color="blue" icon="fa-clock" text="2 minute read" >}}

## Pre-Built Packages

Dissolve is available in packaged / installable form for several common operating systems from the [packages]({{< ref "packages" >}}) page.  Note that these packages focus primarily on providing the GUI version of the code, which can be used for setting up / visualising simulations, as well as running small-scale simulations such as the [examples]({{< ref "examples" >}}).

For serious simulations, the parallel MPI version is recommended, but which must usually be compiled by hand on the target machine on a case-by-case basis.

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
