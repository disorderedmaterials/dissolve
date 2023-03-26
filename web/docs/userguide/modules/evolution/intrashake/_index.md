---
title: IntraShake (Module)
linkTitle: IntraShake
description: Perform Monte Carlo on intramolecular terms
---

## Overview

The `IntraShake` module performs Monte Carlo "shakes" of intramolecular terms within molecules, covering defined bond, angle, and torsions. All moves take into account both the full intermolecular and intramolecular energies of the atoms.

## Description

The basic algorithm is the same as documented for the {{< module "AtomShake" >}} module.

## Options

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Configuration`|`Configuration`|--|{{< required-label >}}Target configuration on which to operate.|

### Control

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`RestrictToSpecies`|`Species ...`|--|Restrict Monte Carlo moves to only molecules of the specified species. Molecules of other species types remain at their current positions.|
|`ShakesPerTerm`|`int`|`1`|Number of shakes $n$ to attempt per term|
|`TargetAcceptanceRate`|`alpha`|`0.33`|Target acceptance rate $\alpha$ for Monte Carlo moves|
|`TermEnergyOnly`|`bool`|`false`|Whether to ignore all interatomic pair potential energy in the procedure, focussing entirely on the energy of the intramolecular term|

### Bonds

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`AdjustBonds`|`bool`|`true`|Whether bond terms should be shaken|
|`BondStepSize`|`delta`|`0.05`|Step size $\delta$ in Angstroms to use in Monte Carlo moves over bonds. As detailed above, the step size is dynamically updated after the module has run, with the updated value being saved in the restart file.|
|`BondStepSizeMax`|`deltamax`|`1.0`|Maximum allowed value for bond step size, $\delta_{max}$, in Angstroms|
|`BondStepSizeMin`|`deltamin`|`0.001`|Minimum allowed value for bond step size, $\delta_{min}$, in Angstroms|

### Angles

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`AdjustAngles`|`bool`|`true`|Whether angle terms should be shaken|
|`AngleStepSize`|`delta`|`0.05`|Step size $\delta$ in Angstroms to use in Monte Carlo moves over angles. As detailed above, the step size is dynamically updated after the module has run, with the updated value being saved in the restart file.|
|`AngleStepSizeMax`|`deltamax`|`1.0`|Maximum allowed value for angle step size, $\delta_{max}$, in Angstroms|
|`AngleStepSizeMin`|`deltamin`|`0.001`|Minimum allowed value for angle step size, $\delta_{min}$, in Angstroms|

### Torsions

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`AdjustTorsions`|`bool`|`true`|Whether torsion terms should be shaken|
|`TorsionStepSize`|`delta`|`0.05`|Step size $\delta$ in Angstroms to use in Monte Carlo moves over torsions. As detailed above, the step size is dynamically updated after the module has run, with the updated value being saved in the restart file.|
|`TorsionStepSizeMax`|`deltamax`|`1.0`|Maximum allowed value for torsion step size, $\delta_{max}$, in Angstroms|
|`TorsionStepSizeMin`|`deltamin`|`0.001`|Minimum allowed value for torsion step size, $\delta_{min}$, in Angstroms|

### Advanced

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`CutoffDistance`|`r`|--|Interatomic cutoff distance $r$ to use for energy calculation. The default is to use the global pair potential cutoff defined in the simulation. If necessary, a short cutoff value can be set during early equilibration runs to significantly speed up calculation times at the expense of realism.|
