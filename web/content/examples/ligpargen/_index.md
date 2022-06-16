---
title: LigParGen Forcefields
description: Importing data from the LigParGen service
type: docs
weight: 100
---

{{< label color="blue" icon="fa-clock" text="15 minutes" >}}

## Summary

As you will have already seen, the requirement of a full forcefield description for each molecule is one of the biggest differences between Dissolve and EPSR. That introduces a barrier in the set-up of your simulation, and while we provide a small selection of common or semi-specific forcefields (which is growing all the time as the code is applied to new problems) there will inevitably be many molecules for which these built-in forcefields aren't suitable.

This example shows some basic usage of the [LigParGen service](http://zarbi.chem.yale.edu/ligpargen/) offered by the Jorgensen group at Yale, from which you can extract a full forcefield for a target molecule, and bring it in to Dissolve.

## Preparation

If, for some reason, the LigParGen service is offline we provide some test data here, and which is also included in the example data ({{< exampledatalink linkText="zip" urlSuffix="zip" >}} or {{< exampledatalink linkText="tar.gz" urlSuffix="tar.gz" >}}).

- Water Species: [water.xyz](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/ligpargen/data/water.xyz)

{{< action type="menu" text="File &#8680; New" />}}
{{< action type="menu" text="File &#8680; Save As..." />}}
{{< step text="Save your input file under a sensible name in same directory as the example data" />}}

{{< button pos="left" text="Let's begin!" path="step1/">}}
