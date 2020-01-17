---
title: Liquid Argon
parent: Examples
has_children: true
---
# Liquid Argon

20 minute read
{: .label .label-blue }

## Summary

Liquid argon isn't a particularly exciting system, but it has been measured experimentally by neutron scattering, and it serves as an excellent example to introduce the basic workflows of Dissolve. This example will show you how to set up a complete working simulation from scratch, with the purpose to simulate liquid argon at 85 K and compare the simulation to reference neutron data from ["Structure Factor and Radial Distribution Function for Liquid Argon at 85 °K"](https://journals.aps.org/pra/abstract/10.1103/PhysRevA.7.2130), J. L. Yarnell, M. J. Katz, R. G. Wenzel, and S. H. Koenig, _Phys. Rev. A_ **7**, 2130.

## Preparation

Download the example data file archive `Dissolve-X.Y.Z-ExampleData.zip` or `Dissolve-X.Y.Z-ExampleData.tar.gz` from the [continuous build](https://github.com/trisyoungs/dissolve/releases/tag/continuous) in the releases section on GitHub if you haven't done so already.

> Open the Dissolve GUI
{: .action .action_dissolve}
> Choose **Empty Simulation** from the _Create_ section
{: .step}
> Save As...
{: .action .action_menu}
> Save your own input file under a sensible name in the `argon` directory of the example data
{: .step}

Alternatively, download the data files listed below and save them to your location of choice.

- Neutron data file: [yarnell.sq](https://raw.githubusercontent.com/trisyoungs/dissolve/develop/examples/argon/data/yarnell.sq)

[Let's begin!](step1.md){: .btn}

