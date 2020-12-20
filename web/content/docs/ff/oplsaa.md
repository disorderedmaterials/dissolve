---
title: OPLS-AA (2005)
description: Optimised potentials for liquid simulations (all-atom version)
---

### Provides

|Forcefield Name|Description|
|:--------------|:----------|
|`OPLSAA2005/Alcohols`|Simple mono-alcohols|
|`OPLSAA2005/Alkanes`|Linear, branched, and cyclic alkanes|
|`OPLSAA2005/Alkenes`|Alkenes and dienes|
|`OPLSAA2005/Aromatics`|Aromatics (e.g. benzene, naphthalene etc.)|
|`OPLSAA2005/Diols`|Diols only|
|`OPLSAA2005/NobleGases`|Noble gases He, Ne, Ar, Kr, and Xe|
|`OPLSAA2005/Triols`|Triols only|

### Overview

Implements the forcefield terms and parameters from OPLS-AA (2005 version) as found in:
- W. L. Jorgensen, D. S. Maxwell, and J. Tirado-Rives, _J. Am. Chem. Soc._ **118**, 11225-11236 (1996).
- W. L. Jorgensen and N. A. McDonald, _Theochem_ **424**, 145-155 (1998).
- W. L. Jorgensen and N. A. McDonald, _J. Phys. Chem. B_ **102**, 8049-8059 (1998).
- R. C. Rizzo and W. L. Jorgensen, _J. Am. Chem. Soc._ **121**, 4827-4836 (1999).
- M. L. Price, D. Ostrovsky, and W. L. Jorgensen, _J. Comp. Chem._ **22**, 1340-1352, (2001).
- E. K. Watkins and W. L. Jorgensen, _J. Phys. Chem. A_ **105**, 4118-4125 (2001).

Automatic type detection is implemented only for specific subsections of the forcefield, each contained within their own "satellite" forcefield as listed above.

### Implementation Notes

#### Atom Types

- Epsilon values have been converted from kcal to kJ.
- Atom types containing '*' (e.g. 'C*', 'N*') are renamed to use '^' as '*' represents a wildcard match in the name.
