---
title: Ionic Liquid (EMIM-OAc)
description: The ionic liquid 1-ethyl-3-imidazolium acetate
weight: 6
type: docs
---

{{< label color="blue" icon="fa-clock" text="60 minutes" >}}

## Summary

This example focuses on setting up an ionic liquid (1-ethyl-3-imidazolium acetate) and reproducing some of the published results in "[Structure and Dynamics of 1-Ethyl-3-methylimidazolium Acetate via Molecular Dynamics and Neutron Diffraction](https://doi.org/10.1021/jp102180q)", D. T. Bowron, C. D’Agostino, L. F. Gladden, C. Hardacre, J. D. Holbrey, M. C. Lagunas, J. McGregor, M. D. Mantle, C. L. Mullan, and T. G. A. Youngs, _J. Phys. Chem. B_ **114**, 7760 (2010). The authors used the SANDALS instrument to collect neutron data on seven isotopic samples at 323 K - the data as provided here have been reprocessed in full, and so may differ slightly from those presented in the original article.

## Preparation

Download the example data as a {{< exampledatalink linkText="zip" urlSuffix="zip" >}} or {{< exampledatalink linkText="tar.gz" urlSuffix="tar.gz" >}} and unpack it if you haven't done so already. Alternatively, download the data files listed below and save them to a location of your choice.

- Cation Coordinates: [emim.xyz](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/emim-oac/species/emim.xyz)
- Anion Coordinates: [oac.xyz](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/emim-oac/species/oac.xyz)
- Fully Protiated IL Data: [`SLS39546-H_H.mint01`](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/emim-oac/data/SLS39546-H_H.mint01)
- Cation-H / Anion-D<sub>3</sub> Data: [`SLS39548-H_D.mint01`](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/emim-oac/data/SLS39548-H_D.mint01)
- Cation-D<sub>8</sub> / Anion-H Data: [`SLS39549-H3D8_H.mint01`](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/emim-oac/data/SLS39549-H3D8_H.mint01)
- Cation-D<sub>8</sub> / Anion-D<sub>3</sub> Data: [`SLS39550-H3D8_D.mint01`](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/emim-oac/data/SLS39550-H3D8_D.mint01)
- Cation-H / Anion-H:D<sub>3</sub> Data: [`SLS39567-H_50.mint01`](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/emim-oac/data/SLS39567-H_50.mint01)
- Cation-H:D<sub>8</sub> / Anion-H:D<sub>3</sub> Data:[`SLS39568-50_50.mint01`](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/emim-oac/data/SLS39568-50_50.mint01)

{{< action type="menu" text="File &#8680; New" />}}
{{< action type="menu" text="File &#8680; Save As..." />}}
{{< step text="Save your input file under a sensible name in same directory as the example data" />}}

{{< button pos="left" text="Let's begin" path="step1/">}}
