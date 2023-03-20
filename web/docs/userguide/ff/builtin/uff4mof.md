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


#### Atom Types

- O_2_z and O_3_f have been changed to O_z_2 and O_f_3 accordingly
- O_m_2 atom type introduced based upon the O_2 atom type in original UFF to describe O_2 atom type bonded to a Carboxylic carbon (C_R) and a metal centre
- If a metallic atom type is found within multiple different secondary building units (SBUs), then duplicate entries of the atom type are found within the code to account for the different geometries and ligands within each of the different SBUs - the parameters are not changed. The affected atom types are: Co4+2; Zn4+2; Zn3f2.
