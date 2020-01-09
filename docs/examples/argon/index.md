---
title: Liquid Argon
parent: Examples
has_children: true
---
# Liquid Argon

20 minute read
{: .label .label-blue }

## Summary

Illustrate basic workflows of Dissolve by simulating liquid argon at 85 K, comparing to reference neutron data from ["Structure Factor and Radial Distribution Function for Liquid Argon at 85 °K"](https://journals.aps.org/pra/abstract/10.1103/PhysRevA.7.2130), J. L. Yarnell, M. J. Katz, R. G. Wenzel, and S. H. Koenig, _Phys. Rev. A_ **7**, 2130.

Steps:
- Create a species to represent an argon atom
- Add suitable interaction parameters
- Define a configuration consistent with experimentally-measured data
- Create processing layers to evolve the configuration, calculate suitable parameters
- Investigate adjusting the interatomic potential by hand

## Data Files
- Neutron data file: [yarnell.sq](https://github.com/trisyoungs/dissolve/tree/develop/examples/argon/data/yarnell.sq)
- Reference input file: [argon.txt](https://github.com/trisyoungs/dissolve/tree/develop/examples/argon/argon.txt)

## Preparation

Create a new directory in a suitable location and download or copy the data files listed above into it.

> Open the Dissolve GUI
{: .action .action_dissolve}
> Choose **Empty Simulation** from the _Create_ section
{: .step}
> Save As...
{: .action .action_menu}
> Save your input file under a sensible name in the directory containing the example data file
{: .step}


[Let's begin!](step1.md){: .btn}

