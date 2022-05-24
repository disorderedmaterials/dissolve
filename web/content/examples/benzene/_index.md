---
title: A Simple Aromatic
description: Structure of liquid benzene from isotopic neutron data
weight: 3
type: docs
---

{{< label color="blue" icon="fa-clock" text="80 minutes" >}}

## Summary

You might always be asking the question "How much does quantity _X_ differ between the equilibrated simulation at the reference forcefield parameters, and the refined simulation at the empirical parameters?". This is a very important question, and this example will show how to checkpoint simulation data for comparison later on. It will also introduce the utility of size factor scaling when dealing with molecular systems containing rings.

You will set up and equilibrate a small liquid benzene simulation before calculating a few properties of interest, and then refine the potential against experimental data and recalculate the properties of interest to see how much they change. The data are three neutron scattering datasets measured on the NIMROD instrument at the ISIS Pulsed Neutron and Muon Source in 2014. We'll be referencing the paper by Headen _et al._ along the way - ["Structure of &pi;-&pi; Interactions in Aromatic Liquids"](https://dx.doi.org/10.1021/ja909084e), T. F. Headen, C. A. Howard, N. T. Skipper, M. A. Wilkinson, D. T. Bowron and A. K. Soper, _J. Am. Chem. Soc._ **132**, 5735 (2010).

## Preparation

Download the example data as a {{< exampledatalink linkText="zip" urlSuffix="zip" >}} or {{< exampledatalink linkText="tar.gz" urlSuffix="tar.gz" >}} and unpack it if you haven't done so already. Alternatively, download the data files listed below and save them to a location of your choice.

- C<sub>6</sub>H<sub>6</sub>: [C6H6.mint01](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/benzene/data/C6H6.mint01)
- C<sub>6</sub>H<sub>6</sub>:C<sub>6</sub>D<sub>6</sub> (50:50): [5050.mint01](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/benzene/data/5050.mint01)
- C<sub>6</sub>D<sub>6</sub>: [C6D6.mint01](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/benzene/data/C6D6.mint01)

{{< action type="menu" text="File &#8680; New" />}}
{{< action type="menu" text="File &#8680; Save As..." />}}
{{< step text="Save your input file under a sensible name in same directory as the example data" />}}

{{< button pos="left" text="Let's begin" path="step1/">}}
