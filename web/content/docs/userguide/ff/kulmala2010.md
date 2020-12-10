---
title: Kulmala (2010)
description: Sulfuric acid / ammonia / water
---

### Provides

|Forcefield Name|Description|
|:--------------|:----------|
|`Kulmala2010`|Full forcefield|

### Overview

Implements forcefield found in "[Enhancing effect of dimethylamine in sulfuric acid nucleation in the presence of water – a computational study](https://dx.doi.org/10.5194/acp-10-4961-2010)", V. Loukonen, T. Kurten, I. K. Ortega, H. Vehkamaki, A. A. H. Padua, K. Sellegri, and M. Kulmala, _Atmos. Chem. Phys._ **10**, 4961 (2010).

The forcefield covers water (SPC/E), the hydronium ion, ammonia, the ammonium ion, sulfuric acid, the hydrogensulfate ion, and the dimethylammonium ion. Based on OPLS-AA parameters, and augmented by QM-derived values.

### Implementation Notes

#### Intramolecular Parameters

- Bond term parameters for HCD-CDM and angle term parameters for HCD-CDM-HCD are not present in the SI, and so are taken from the original OPLS-AA forcefield.
