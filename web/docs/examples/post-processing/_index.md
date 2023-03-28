---
title: Post Processing
description: Using Dissolve as a post-processor
type: docs
weight: 101
---

{{< label color="blue" icon="fa-clock" text="20 minutes" >}}

## Summary

Sometimes it's useful to apply Dissolve's analysis capabilities to external data, e.g. from a molecular dynamics simulation. Dissolve can import coordinates from single configuration "snapshots" or from trajectories, and can thus apply any of its functions (e.g. calculation of weighted structure factors, application of analysis layers) in the standard way. Essentially, this involves setting up a standard simulation in Dissolve but instead of evolving the system with an evolution layer, we use an import layer instead.

Just as with any standard simulation in Dissolve, species, their isotopologues (if required), and analysis sites must be defined in order to calculate what you need. In particular, the order of atoms within species must match those in the data file, as must the ordering of species/molecules in the configuration the data file represents. The only thing that isn't really important is which forcefield you apply to the species, since we won't be performing any evolution on the system ourselves. However, the forcefield of course guides the atom type that we see, and so has a direct effect on, for instance, the {{< module "NeutronSQ" >}} and is output.

For this example we have a pre-run DL\_POLY molecular dynamics simulation of 9 mol% isopropanol in water which we'll import into Dissolve.

## Preparation

Download the example data as a {{< exampledatalink linkText="zip" urlSuffix="zip" >}} or {{< exampledatalink linkText="tar.gz" urlSuffix="tar.gz" >}} and unpack it if you haven't done so already. Alternatively, download the data files listed below and save them to a location of your choice.

- Water Species: [water.xyz](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/post-processing/data/water.xyz)
- Isopropanol Species: [ipa.xyz](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/post-processing/data/ipa.xyz)
- 9 mol% IPA/Water Trajectory: [9molpcnt-IPA-water.xyz](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/post-processing/data/9molpcnt-IPA-water.xyz)

{{< action type="menu" text="File &#8680; New" />}}
{{< action type="menu" text="File &#8680; Save As..." />}}
{{< step text="Save your input file under a sensible name in same directory as the example data" />}}

{{< button pos="left" text="Let's begin!" path="step1/">}}
