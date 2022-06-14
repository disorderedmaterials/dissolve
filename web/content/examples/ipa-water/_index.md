---
title: Isopropanol & Water
description: A mixture of isopropanol and water
weight: 7
type: docs
---

{{< label color="blue" icon="fa-clock" text="90 minutes" >}}

## Summary

Most of the core Dissolve workflow has been illustrated in the previous tutorials, here we will apply that knowledge to analyse a 90 mol% water 2-propanol/water mixture. This tutorial will serve as the basis for other mixed liquid systems.
The data are five neutron scattering datasets measured on the SANDALS instrument at the ISIS Pulsed Neutron and Muon Source in 2014. We’ll be referencing the paper [“Structure and dynamics of aqueous 2-propanol: a THz-TDS NMR and neutron diffraction study”](https://pubs.rsc.org/en/content/articlelanding/2015/cp/c5cp01132a), J. McGregor, R. Li, A. Zeitler, C. D’Agostino, J. H. P. Collins, M. D. M. Mantle, J. D. Holbrey, M. Falkowska, T. G. A. Youngs, C. Hardacre, E. H. Stitt and L. F. Gladden, *Phys. Chem. Chem. Phys*. **17**, 30481 (2015).

## Preparation

Download the example data as a [zip]() or [tar.gz]() and unpack it if you haven’t done so already. Alternatively, download the data files listed below and save them to a location of your choice.
- [9:1_D2O:IPA-d8](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/ipa-water/data/9:1_D2O:IPA-d8)
- [9:1_D2O:IPA-d6](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/ipa-water/data/9:1_D2O:IPA-d6)
- [9:1_D2O:IPA-h8](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/ipa-water/data/9:1_D2O:IPA-h8)
- [9:1_H2O:IPA-d8](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/ipa-water/data/9:1_H2O:IPA-d8)
- [9:1_H2O:IPA-h8](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/ipa-water/data/9:1_H2O:IPA-h8)

{{< action type="menu" text="File &#8680; New" />}}
{{< action type="menu" text="File &#8680; Save As..." />}}
{{< step text="Save your input file under a sensible name in same directory as the example data" />}}
{{< button pos="left" text="Let's begin" path="step1/" >}}
