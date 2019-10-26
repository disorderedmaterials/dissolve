---
title: Getting Dissolve
parent: Getting Started
nav_order: 20
---
# Getting Dissolve

Dissolve is available in serial and parallel versions, along with a separate GUI. You will find separate packages for all three.

## Pre-built Binaries

Dissolve is available in pre-built form for most platforms. You can find installers and AppImages for major releases as well as the current development version on the [releases page](https://github.com/trisyoungs/dissolve/releases) at GitHub.

## Source Code

The source code is available on GitHub as the [Dissolve project](https://github.com/trisyoungs/dissolve). See the [build instructions](compilation.md) for more information.

## SCARF

The parallel version of Dissolve is installed as a module on [SCARF](https://www.scarf.rl.ac.uk/home).

To see the available versions, run:

```
bob@scarf:~> module avail dissolve

------------------- /apps/modulefiles --------------------
   contrib/dissolve/1.0.0
   contrib/dissolve/1.1.0

--------------- /apps/contrib/modulefiles ----------------
   dissolve/1.0.0
   dissolve/1.1.0
```

To source one ready for use run:

```
bob@scarf:~> module load contrib/dissolve/1.1.0
```

#### TODO
- Example script.
- Notes on / link to parallelism info, mpirun etc.
