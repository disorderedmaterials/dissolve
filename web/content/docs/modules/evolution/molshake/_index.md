---
title: MolShake (Module)
linkTitle: MolShake
description: Perform Monte Carlo on whole molecules
---

## Overview

The `MolShake` module performs whole-molecule Monte Carlo "shakes" of every molecule in each of the supplied target configurations. The shakes comprise both rotational and translational moves, during which the internal geometry of the molecule is kept fixed.

## Description

### Basic Algorithm

For each molecule $m$ in a given configuration of $N$ molecules, the basic sequence of operations performed by `MolShake` is as follows:

1. Calculate the reference energy $E_{ref}$ arising from all interactions involving $m$
2. Generate a random displacement vector $v$ and rotation matrix $R$
3. Generate new coordinates for $m$ by moving it along $v$ by a distance (or step size) $\delta_r$ and simultaneously rotating it using the rotation matrix $R$ by an angle step $\delta_\theta$
4. Calculate the new energy $E_{new}$
5. Calculate the energy delta $\Delta E = E_{new} - E_{ref}$. If $\Delta E$ is negative, accept the new coordinates of $m$ and move to the next molecule. Otherwise, only accept the new coordinates if $\chi \lt \exp{-\frac{\Delta E}{RT}}$, where $\chi$ is a random real number between 0 and 1, $R$ is the gas constant, and $T$ is the temperature of the configuration in Kelvin.

### Acceptance Rates

The `MolShake` module uses independent step sizes for the translational and rotational components of the trial move. In order to adjust these step sizes, 10% of trial moves are made as pure rotations, and a further 10% as pure translations, with the remaining 80% being a combination of the two move types. Once moves have been attempted for all $N$ molecules in one configuration, the actual acceptance rates for the two move components are

$$ \alpha_{actual}^{trans} = \frac{N_{accepted}^{trans}}{N} $$

and

$$ \alpha_{actual}^{rot} = \frac{N_{accepted}^{rot}}{N} $$

The individual step sizes $\delta^{trans}$ and $\delta^{rot}$ can then be adjusted in order to tend towards the requested acceptance ratio, $\alpha$, in the next cycle:

$$ \delta_{new}^{trans} = \delta_{old}^{trans} \frac{\alpha_{actual}^{trans}}{\alpha} $$

and

$$ \delta_{new}^{rot} = \delta_{old}^{rot} \frac{\alpha_{actual}^{rot}}{\alpha} $$

If it occurs that no moves are accepted for either of the move types, the corresponding step size is multiplied by a factor of 0.8 instead of using the above equations. Following adjustment of the step sizes they are clamped such that $\delta_{min} \le \delta_{new} \le \delta_{max}$.

## Configuration

### Control Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`CutoffDistance`|`r`|--|Interatomic cutoff distance $r$to use for energy calculation. The default is to use the global pair potential cutoff defined in the simulation. If necessary, a short cutoff value can be set during early equilibration runs to significantly speed up calculation times at the expense of realism.|
|`RotationStepSize`|`delta`|`1.0`|Step size $\delta$ in degrees to use for the rotational component of the Monte Carlo moves. As detailed above, the step size is dynamically updated after the module has run, with the updated value being saved in the restart file.|
|`RotationStepSizeMax`|`deltamax`|`90.0`|Maximum allowed value for rotational step size,  $\delta^{rot}_{max}$, in Angstroms|
|`RotationStepSizeMin`|`deltamin`|`0.01`|Minimum allowed value for rotational step size, $\delta^{rot}_{min}$, in Angstroms|
|`ShakesPerAtom`|`n`|`1`|Number of shakes $n$ to attempt per atom|
|`TargetAcceptanceRate`|`alpha`|`0.33`|Target acceptance rate $\alpha$ for Monte Carlo moves|
|`TranslationStepSize`|`delta`|`0.05`|Step size $\delta$ in Angstroms for the translational component of the Monte Carlo moves. As detailed above, the step size is dynamically updated after the module has run, with the updated value being saved in the restart file.|
|`TranslationStepSizeMax`|`deltamax`|`1.0`|Maximum allowed value for translational step size, $\delta^{trans}_{max}$, in Angstroms|
|`TranslationStepSizeMin`|`deltamin`|`0.001`|Minimum allowed value for translational step size, $\delta^{trans}_{min}$, in Angstroms|


|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Name`|`type`|`def`|Text|
