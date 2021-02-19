---
title: UFF
description: Universal force field of Rappe (1992)
---

### Provides

|Forcefield Name|Description|
|:--------------|:----------|
|`UFF`|Full forcefield|

### Overview

Implements the UFF forcefield as described in "[UFF, a Full Periodic Table Force Field for Molecular Mechanics and Molecular Dynamics Simulations](https://doi.org/10.1021/ja00051a040)", A. K. Rappe, C. J. Casewit, K. S. Colwell, W. A. Goddard III, and W. M. Skiff, _J. Am. Chem. Soc._ **114**, 10024-10039 (1992).

### Implementation Notes

Implementation of the UFF is not a trivial task as there are (documented) errors in the original manuscript. Much of the hard work in identifying and correcting these errors has been performed by Marcus G. Martin and collaborators during implementation of UFF in the [MCCCS Towhee](http://towhee.sourceforge.net/) code, for which details can be found at http://towhee.sourceforge.net/forcefields/uff.html. Nevertheless, there are still implementation details that are missing, and so have largely been taken on faith direct from the MCCCS Towhee source code itself where necessary.

#### Atom Types

- Dissolve introduces specific atom types for the amide bond - `C_amR` and `N_amR` - for ease of type identification. These atoms types possess identical parameters to their `C_R` and `N_R` counterparts, but are detected explicitly in the term generation routines.
