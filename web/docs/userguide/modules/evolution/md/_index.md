---
title: MD (Module)
linkTitle: MD
description: Run a short molecular dynamics simulation
---

## Overview

The `MD` module performs a molecular dynamics run on each supplied target configuration, and can be used to relax the intramolecular geometry of molecules and promote their "diversity".

## Description

### Basic Algorithm

The `MD` module employs a standard two-stage velocity Verlet algorithm as follows. Initially, a set of random velocities $v^r$ is assigned to the atoms, with each vector component given a random value between {-0.5, 0.5}. These velocities are then adjusted so that no velocity shift in the system is present (i.e. the periodic box contents as a whole are not permitted to "drift") and scaled to represent the target temperature $T$:

$$ v_i(0) = \left(v^r_i - \frac{\sum_i{v^r_i m_i}}{\sum_i{m_i}}\right) \sqrt{\frac{T}{ \frac{2}{3 N k_b}\sum_i{\frac{1}{2} m_i(v_i \cdot v_i)}}} $$

where $m_i$ is the mass of the $i$th atom, $N$ is the number of atoms in the system, and $k_b$ is Boltzmann's constant.

In the first stage of the velocity Verlet, at a given time $t$ the positions $r$ of all atoms are evolved by a full step according to the current timestep $\Delta t$:

$$ r_i(t + \Delta t) = r_i(t) + v_i(t)\Delta t + \frac{1}{2}a_i(t)\Delta t^2 $$

where $a_i$ is the acceleration of the $i$th atom (initially zero). The velocities are evolved by half a step as follows:

$$ v_i\left(t + \frac{1}{2}\Delta t\right) = v_i(t) + \frac{1}{2}a_i(t)\Delta t $$

At this point, the forces $f$ on the atoms are calculated and the new accelerations of the atoms determined...

$$ a_i\left(t + \Delta t\right) = \frac{f_i(t)}{m_i} $$

... and the velocities evolved by their remaining half-step using the new accelerations and re-scaled to give the desired temperature:

$$ v_i\left(t + \Delta t\right) = v_i\left(t + \frac{1}{2}\Delta t\right) + \frac{1}{2} a_i(t + \Delta t) \Delta t $$

$$ v_i\left(t + \Delta t, T\right) = v_i\left(t + \Delta t\right) \sqrt{\frac{T}{ \frac{2}{3 N k_b}\sum_i{\frac{1}{2} m_i\left(v_i\left(t + \Delta t, T\right) \cdot v_i\left(t + \Delta t, T\right)\right)}}} $$

## Options

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Configuration`|`Configuration`|--|{{< required-label >}}Target configuration on which to operate.|

### Simulation

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
`NSteps`|`int`|`50`|Number of molecular dynamics steps to perform|
|`Timestep`|[`TimestepType`]({{< ref "timesteptype" >}})|`Auto`|Timestep type / strategy to use in the calculation.|
|`DeltaT`|`double`|`5.0e-4`|Timestep to use in the simulation (if the timestep style utilises one).|
|`RandomVelocities`|`bool`|`false`|Whether to always assign random velocities when starting the molecular dynamics simulation. If `false` then random velocities are only generated if no other velocities exist.|

### Control

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`OnlyWhenEnergyStable`|`bool`|`true`|Only run molecular dynamics on a configuration if the total energy of the configuration (as determined by an [`Energy`]({{< ref "energy" >}}) module is considered stable.|
|`RestrictToSpecies`|`Species ...`|--|Restrict force calculation to only molecules of the specified species. Molecules of other species types remain at their current positions (always have zero force acting on them).|

### Output

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
`EnergyFrequency`|`int`|`10`|Frequency at which to calculate total system energy|
`OutputFrequency`|`int`|`5`|Frequency at which to output step information|
`TrajectoryFrequency`|`int`|`0`|Write frequency for trajectory file|

### Advanced

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`CapForces`|`bool`|`false`|Whether to cap forces acting on atoms to a predefined limit, preventing potential explosions when bad contacts are present in the system. The value at which to cap forces is set by the `CapForcesAt` keyword.|
|`CapForcesAt`|`force`|`1.0e7`|Value (in 10 J/mol) at which to cap forces if `CapForces` is enabled.|
|`CutoffDistance`|`r`|--|Interatomic cutoff distance $r$ to use for energy and force calculation. The default is to use the global pair potential cutoff defined in the simulation. If necessary, a short cutoff value can be set during early equilibration runs to significantly speed up calculation times at the expense of realism.|
|`IntraOnly`|`bool`|`false`|Only calculate forces arising from internal molecule interactions (i.e. bonds, angles, torsion, impropers, and any allowed pair potential contributions) and ignore forces between molecules. This can be useful to force efficient exploration of intramolecular degrees of freedom at the expense of molecule-molecule interactions. If used, a subsequent relaxation with [`MolShake`]({{< ref "molshake" >}}) is highly recommended.|
