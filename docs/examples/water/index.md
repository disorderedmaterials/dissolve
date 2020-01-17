---
title: Liquid Water
parent: Examples
has_children: true
nav_order: 3
---
# Liquid Water

60 - 75 minutes
{: .label .label-blue }

## Summary

Run, refine, and analyse the structure of liquid water at 298 K using three neutron scattering datasets measured on the SANDALS diffractometer at the ISIS Pulsed Neutron and Muon Source in 2001

Steps:
- Set up a suitable species and configuration
- Equilibrate the system
- Adjust intramolecular geometry to match experiment
- Refine the interatomic potentials
- Analyse the liquid structure of the refined simulation

## Preparation

Download the example data file archive `Dissolve-X.Y.Z-ExampleData.zip` or `Dissolve-X.Y.Z-ExampleData.tar.gz` from the [continuous build](https://github.com/trisyoungs/dissolve/releases/tag/continuous) in the releases section on GitHub if you haven't done so already.

> Open the Dissolve GUI
{: .action .action_dissolve}
> Choose **Empty Simulation** from the _Create_ section
{: .step}
> Save As...
{: .action .action_menu}
> Save your own input file under a sensible name in the `water` directory of the example data
{: .step}

Alternatively, download the data files listed below and save them to your location of choice.

- H<sub>2</sub>O: [SLS18498-H2O.mint01](https://raw.githubusercontent.com/trisyoungs/dissolve/develop/examples/water/data/SLS18498-H2O.mint01)
- HDO: [SLS18500-HDO5050.mint01](https://raw.githubusercontent.com/trisyoungs/dissolve/develop/examples/water/data/SLS18500-HDO5050.mint01)
- D<sub>2</sub>O: [SLS18502-D2O.mint01](https://raw.githubusercontent.com/trisyoungs/dissolve/develop/examples/water/data/SLS18502-D2O.mint01)

[Let's begin!](step1.md){: .btn }
