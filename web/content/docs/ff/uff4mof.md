---
title: UFF4MOF
description: Extension of UFF to MOFs (Addicoat 2014)
---

### Provides

|Forcefield Name|Description|
|:--------------|:----------|
|`UFF4MOF`|Full forcefield|

### Overview

Implements "[Extension of the Universal Force Field to Metal-Organic Frameworks](https://doi.org/10.1021/ct400952t)" by M. A. Addicoat, N. Vankova, I. F. Akter, T. Heine J. Chem. Theory Comput. 2014, 10, 2, 880–891

### Implementation Notes

Placeholder

#### Atom Types

Dissolve introduces duplicate entries of specific atom types to describe their use in multiple secondary building units (SBUs)
- O_2_z and O_3_f have been changed to O_z_2 and O_f_3
- O_m_2 atom type introduced based upon the O_2 atom type in original UFF to describe O_2 atom type bonded to a Carboxylic carbon (C_R) and a metal centre
