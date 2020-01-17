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

You might always be asking the question "How much does quantity _X_ differ between the equilibrated simulation at the reference forcefield parameters, and the refined simulation at the empirical parameters?". This is a very fair question, and this example will show how to checkpoint simulation data for comparison later on using some custom workspace graphs. It will also introduce the utility of size factor scaling when dealing with molecular systems containing rings.

You will set up and equilibrate a small liquid benzene simulation before calculating a few properties of interest, and then refine the potential against experimental data and recalculate the properties of interest before graphing both side-by-side. The data are three neutron scattering datasets measured on the NIMROD diffractometer at the ISIS Pulsed Neutron and Muon Source in 2014.

## Preparation

Download the example data file archive `Dissolve-X.Y.Z-ExampleData.zip` or `Dissolve-X.Y.Z-ExampleData.tar.gz` from the [continuous build](https://github.com/trisyoungs/dissolve/releases/tag/continuous) in the releases section on GitHub if you haven't done so already.

> Open the Dissolve GUI
{: .action .action_dissolve}
> Choose **Empty Simulation** from the _Create_ section
{: .step}
> Save As...
{: .action .action_menu}
> Save your own input file under a sensible name in the `benzene` directory of the example data
{: .step}

Alternatively, download the data files listed below and save them to your location of choice.

- C<sub>6</sub>H<sub>6</sub>: [C6H6.mint01](https://raw.githubusercontent.com/trisyoungs/dissolve/develop/examples/benzene/data/C6H6.mint01)
- C<sub>6</sub>H<sub>6</sub>:C<sub>6</sub>D<sub>6</sub> (50:50): [5050.mint01](https://raw.githubusercontent.com/trisyoungs/dissolve/develop/examples/benzene/data/5050.mint01)
- C<sub>6</sub>D<sub>6</sub>: [C6D6.mint01](https://raw.githubusercontent.com/trisyoungs/dissolve/develop/examples/benzene/data/C6D6.mint01)

[Let's begin!](step1.md){: .btn }
