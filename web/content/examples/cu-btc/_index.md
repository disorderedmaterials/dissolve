---
title: A Metal Organic Framework
description: Setting up a metal organic framework system
weight: 11
type: docs
---

{{< label color="blue" icon="fa-clock" text="60 minutes" >}}

## Summary

In this example we'll do some baseline comparison between calculated Bragg scattering from a metal organic framework (MOF) and experimental neutron data measured on the NIMROD instrument in 2013.

We'll go through the steps of importing a crystal structure from a Crystallographic Information File (CIF) file, and setting up a basic simulation so we can compare the calculated structure against some experimental neutron data. The target framework is Cu-BTC (copper(II)-benzene-1,3,5-tricarboxylate, known also as MOF-199, HKUST-1, or Basolite C300) and we'll use a structure from the [Crystallography Open Database](http://www.crystallography.net/cod/7108574.html) - the corresponding CIF file is also included in the example file data for Dissolve.

## Preparation

Download the example data as a {{< exampledatalink linkText="zip" urlSuffix="zip" >}} or {{< exampledatalink linkText="tar.gz" urlSuffix="tar.gz" >}} and unpack it if you haven't done so already. Alternatively, download the data files listed below and save them to a location of your choice.

- CIF file: [7108574.cif](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/cu-btc/7108574.cif)
- Neutron Data (Empty MOF): [Empty-CuBTC.mint01](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/cu-btc/data/Empty-CuBTC.mint01)

{{< action type="menu" text="File &#8680; New" />}}
{{< action type="menu" text="File &#8680; Save As..." />}}
{{< step text="Save your input file under a sensible name in same directory as the example data" />}}

{{< button pos="left" text="Let's begin" path="step1/">}}
