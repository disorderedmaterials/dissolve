---
title: Liquid Water
description: Structure of liquid water at 298 K, refining against three isotopically-labelled neutron datasets
type: docs
weight: 2
---

{{< label color="blue" icon="fa-clock" text="60 minutes" >}}

## Summary

It should be no surprise that water is present in a significant fraction of disordered materials studies, owing to its importance to just about everything, and its presence just about everywhere! Here we'll set up, run, refine, and analyse the structure of liquid water at 298 K using three neutron scattering datasets measured on the SANDALS diffractometer at the ISIS Pulsed Neutron and Muon Source in 2001. This will illustrate most of the core workflows in Dissolve when applying it to the analysis of neutron scattering data, and is an excellent place to start if you're new to the analysis of disordered materials.

## Preparation

Download the example data as a {{< exampledatalink linkText="zip" urlSuffix="zip" >}} or {{< exampledatalink linkText="tar.gz" urlSuffix="tar.gz" >}} and unpack it if you haven't done so already. Alternatively, download the data files listed below and save them to a location of your choice.

- H<sub>2</sub>O: [SLS18498-H2O.mint01](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/water/data/SLS18498-H2O.mint01)
- HDO: [SLS18500-HDO5050.mint01](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/water/data/SLS18500-HDO5050.mint01)
- D<sub>2</sub>O: [SLS18502-D2O.mint01](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/water/data/SLS18502-D2O.mint01)

{{< action type="menu" text="File &#8680; New" />}}
{{< action type="menu" text="File &#8680; Save As..." />}}
{{< step text="Save your input file under a sensible name in same directory as the example data" />}}

{{< button pos="left" text="Let's begin!" path="step1/">}}
