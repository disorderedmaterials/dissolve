---
title: Liquid Argon
parent: Learn Dissolve
has_children: true
nav_order: 3
---
# Liquid Argon

20 minute read
{: .label .label-blue }

## Summary

- Illustrate basic workflows of Dissolve
- Liquid argon at 85 K
- Single neutron dataset
  - ["Structure Factor and Radial Distribution Function for Liquid Argon at 85 °K"](https://journals.aps.org/pra/abstract/10.1103/PhysRevA.7.2130), J. L. Yarnell, M. J. Katz, R. G. Wenzel, and S. H. Koenig, _Phys. Rev. A_ **7**, 2130.
  -  Neutron data file: [yarnell.sq](https://github.com/trisyoungs/dissolve/tree/develop/examples/argon/data/yarnell.sq)
- Reference input file: [argon.txt](https://github.com/trisyoungs/dissolve/tree/develop/examples/argon/argon.txt)

We'll now set up a complete, simple simulation by hand to illustrate the basic usage of the GUI, and discuss a few topics in a bit more detail. Our target will be to describe liquid argon at 85 K, beginning from a set of reference interaction parameters, and then to refine this potential against some experimentally-measured data.

Steps:
- Create a species to represent an argon atom
- Add suitable interaction parameters
- Define a configuration consistent with experimentally-measured data
- Create processing layers to evolve the configuration, calculate suitable parameters, and refine the interatomic potential

So, fire up the Dissolve GUI, choose _Create a New Simulation from Scratch_{: .text-green-100}, and we'll get started.

[Let's begin!](step1.md){: .btn }

