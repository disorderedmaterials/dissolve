---
title: AtomShake (Module)
linkTitle: AtomShake
description: Perform Monte Carlo on individual atoms
---

## Overview

The `AtomShake` module performs an atomic Monte Carlo "shake" of every atom in each of the supplied target configurations. All moves take into account both the full intermolecular and intramolecular energies of the atoms.

## Description

### Basic Algorithm

For each atom $i$ in a given configuration of $N$ atoms, the basic sequence of operations performed by `AtomShake` is as follows:

1. Calculate the reference energy $E_{ref}$ arising from all interactions involving $i$
2. Generate a random displacement vector $v$
3. Generate new coordinates for $i$ by moving it along $v$ by a distance (or step size) $\delta$
4. Calculate the new energy $E_{new}$
5. Calculate the energy delta $\Delta E = E_{new} - E_{ref}$. If $\Delta E$ is negative, accept the new coordinates of $i$ and move to the next atom. Otherwise, only accept the new coordinates if $\chi \lt \exp{-\frac{\Delta E}{RT}}$, where $\chi$ is a random real number between 0 and 1, $R$ is the gas constant, and $T$ is the temperature of the configuration in Kelvin.

### Acceptance Rates

Once the algorithm has attempted moves for all atoms in one configuration, the actual acceptance rate $\alpha_{actual}$ can be calculated as

$$ \alpha_{actual} = \frac{N_{accepted}}{N} $$

The step size $\delta$ can then be adjusted in order to tend towards the requested acceptance ratio, $\alpha$, in the next cycle:

$$ \delta_{new} = \delta_{old} \frac{\alpha_{actual}}{\alpha} $$

If it occurs that no moves are accepted, the step size is multiplied by a factor of 0.8 instead of using the above equation. Following adjustment of the step size it is clamped such that $\delta_{min} \le \delta_{new} \le \delta_{max}$.

## Configuration

### Control Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`CutoffDistance`|`r`|--|Interatomic cutoff distance $r$ to use for energy calculation. The default is to use the global pair potential cutoff defined in the simulation. If necessary, a short cutoff value can be set during early equilibration runs to significantly speed up calculation times at the expense of realism.|
|`ShakesPerAtom`|`n`|`1`|Number of shakes $n$ to attempt per atom|
|`StepSize`|`delta`|`0.05`|Step size $\delta$ in Angstroms to use in Monte Carlo moves. As detailed above, the step size is dynamically updated after the module has run, with the updated value being saved in the restart file.|
|`StepSizeMax`|`deltamax`|`1.0`|Maximum allowed value for step size, $\delta_{max}$, in Angstroms|
|`StepSizeMin`|`deltamin`|`0.001`|Minimum allowed value for step size, $\delta_{min}$, in Angstroms|
|`TargetAcceptanceRate`|`alpha`|`0.33`|Target acceptance rate $\alpha$ for Monte Carlo moves|
