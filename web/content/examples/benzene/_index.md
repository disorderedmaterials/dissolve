---
title: A Simple Aromatic
description: Structure of liquid benzene from isotopic neutron data
weight: 3
type: docs
---

70 - 90 minutes
{: .label .label-blue }

## Summary

You might always be asking the question "How much does quantity _X_ differ between the equilibrated simulation at the reference forcefield parameters, and the refined simulation at the empirical parameters?". This is a very fair question, and this example will show how to checkpoint simulation data for comparison later on using some custom workspace graphs. It will also introduce the utility of size factor scaling when dealing with molecular systems containing rings.

You will set up and equilibrate a small liquid benzene simulation before calculating a few properties of interest, and then refine the potential against experimental data and recalculate the properties of interest before graphing both side-by-side. The data are three neutron scattering datasets measured on the NIMROD diffractometer at the ISIS Pulsed Neutron and Muon Source in 2014.

## Preparation

Download the example data as a {{< exampledatalink linkText="zip" urlSuffix="zip" >}} or {{< exampledatalink linkText="tar.gz" urlSuffix="tar.gz" >}} and unpack it if you haven't done so already. Alternatively, download the data files listed below and save them to a location of your choice.

- C<sub>6</sub>H<sub>6</sub>: [C6H6.mint01](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/benzene/data/C6H6.mint01)
- C<sub>6</sub>H<sub>6</sub>:C<sub>6</sub>D<sub>6</sub> (50:50): [5050.mint01](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/benzene/data/5050.mint01)
- C<sub>6</sub>D<sub>6</sub>: [C6D6.mint01](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/benzene/data/C6D6.mint01)

{{< action type="menu" text="File &#8680; New" />}}
{{< action type="menu" text="File &#8680; Save As..." />}}
{{< step text="Save your input file under a sensible name in same directory as the example data" />}}

{{< button pos="left" text="Let's begin" path="step1/">}}
