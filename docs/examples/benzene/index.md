---
title: Comparing Benzene Structure
parent: Examples
has_children: true
nav_order: 4
---
# Comparing Benzene Structure

60 - 75 minutes
{: .label .label-blue }

## Summary

Run, refine, and analyse the structure of liquid benzene at 298 K using three neutron scattering datasets measured on the NIMROD diffractometer at the ISIS Pulsed Neutron and Muon Source in 2014, comparing the structure of the liquid between equilibrated and refined simulations.

Steps:
- Set up a suitable species and configuration
- Equilibrate the system
- Perform a structural analysis of the equilibrated simulation
- Refine the interatomic potentials
- Perform a structural analysis of the refined simulation and compare to the equilibrated data

## Data Files
- C<sub>6</sub>H<sub>6</sub>: [C6H6.mint01](https://github.com/trisyoungs/dissolve/tree/develop/examples/benzene/data/C6H6.mint01)
- C<sub>6</sub>H<sub>6</sub>:C<sub>6</sub>D<sub>6</sub> (50:50): [5050.mint01](https://github.com/trisyoungs/dissolve/tree/develop/examples/benzene/data/5050.mint01)
- C<sub>6</sub>D<sub>6</sub>: [C6D6.mint01](https://github.com/trisyoungs/dissolve/tree/develop/examples/benzene/data/C6D6.mint01)
- Reference input file: [benzene.txt](https://github.com/trisyoungs/dissolve/tree/develop/examples/benzene/benzene.txt)

## Preparation

Create a new directory in a suitable location and download or copy the data files listed above into it.

> Open the Dissolve GUI
{: .action .action_dissolve}
> Choose **Empty Simulation** from the _Create_ section
{: .step}
> Save As...
{: .action .action_menu}
> Save your input file under a sensible name in the directory containing the example data files
{: .step}

[Let's begin!](step1.md){: .btn }
