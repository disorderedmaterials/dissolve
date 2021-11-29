---
title: Zhang (2013)
description: ZIF-8
---

### Provides

|Forcefield Name|Description|
|:--------------|:----------|
|`Zhang2013`|ZIF-8 forcefield (no adsorbates)|

### Overview

Implements ZIF-8 forcefield found in "[Sorption-Induced Structural Transition of Zeolitic Imidazolate Framework-8: A Hybrid Molecular Simulation Study](https://dx.doi.org/10.1021/ja401129h)", L. Zhang, Z. Hu and J. Jiang, _J. Am. Chem. Soc._ **135**, 3722 (2013).

Covers only ZIF-8 parameters listed in the supplementary information. Sorbate parameters are not included.

### Implementation Notes

#### Intramolecular Parameters

- Improper term atoms are reordered from the published values to put the central atom first in the list.
- Torsion between `H2`-`C2`-`N`-`Zn` is required but not listed in the paper, so is added with zero parameters.
