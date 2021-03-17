---
title: Ludwig Ionic Liquids
description: 1‐(4‐hydroxybutyl)pyridinium and 1‐pentylpyridinium cations with NTf2 anion
---

### Provides

|Forcefield Name|Description|
|:--------------|:----------|
|`Ludwig/NTf2`|Bistrifylamide (NTf2) anion|
|`Ludwig/Py4OH`|1‐(4‐hydroxybutyl)pyridinium cation|
|`Ludwig/Py5`|1-pentylpyridinium cation|

{{< warn text="Atomic charges in these forcefields are defined in the top-level atom descriptions, rather than in the atom types. As such, when using these forcefields you must turn off inclusion of charges into the tabulated pair potentials, e.g. by setting `IncludeCoulomb` in the [`PairPotentials`](/docs/inputfile/pairpotentialsblock/) block to `false`, so that the atomic charges assigned to the atoms are used instead." >}}

### Overview

Implements forcefields from:

- "[Revisiting imidazolium based ionic liquids: Effect of the conformation bias of the [NTf2] anion studied by molecular dynamics simulations](http://doi.org/10.1063/1.5013096)", J. Neumann, B. Golub, LM. Odebrecht, R. Ludwig, D. Paschek, _Journal of Chemical Physics_, **148**, 193828 (2018).
