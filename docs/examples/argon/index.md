---
title: Liquid Argon
parent: Examples
has_children: true
---
# Liquid Argon

20 minute read
{: .label .label-blue }

## Overview

- Illustrate basic workflows of Dissolve
- Liquid argon at 85 K
- Single neutron dataset
  - ["Structure Factor and Radial Distribution Function for Liquid Argon at 85 °K"](https://journals.aps.org/pra/abstract/10.1103/PhysRevA.7.2130), J. L. Yarnell, M. J. Katz, R. G. Wenzel, and S. H. Koenig, _Phys. Rev. A_ **7**, 2130.
- Reference input file: [argon.txt](https://github.com/trisyoungs/dissolve/tree/develop/examples/argon/argon.txt)

### Data Files
- Neutron data file: [yarnell.sq](https://github.com/trisyoungs/dissolve/tree/develop/examples/argon/data/yarnell.sq)

## Summary

We'll now set up a complete, simple simulation by hand to illustrate the basic usage of the GUI, and discuss a few topics in a bit more detail. Our target will be to describe liquid argon at 85 K, beginning from a set of reference interaction parameters, and then to refine this potential against some experimentally-measured data.

Steps:
- Create a species to represent an argon atom
- Add suitable interaction parameters
- Define a configuration consistent with experimentally-measured data
- Create processing layers to evolve the configuration, calculate suitable parameters
- Investigate adjusting the interatomic potential by hand

## Preparation

Create a new directory in a suitable location and download or copy the data files listed above into it.

> Open the Dissolve GUI
{: .action .action_dissolve}
> Choose **Create a New Simulation from Scratch**
{: .step}
> Save As...
{: .action .action_menu}
> Save your input file under a sensible name in the directory containing the example data file
{: .step}

[Let's begin!](step1.md){: .btn .right}

