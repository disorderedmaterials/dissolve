---
title: Energy (Module)
linkTitle: Energy
description: Calculate total system energy and its contributions
---

## Overview

The `Energy` module calculates the contributions to the total energy of one or more configurations arising from intramolecular and intermolecular interactions between atoms. Other modules may use these calculated energies to determine whether a particular configuration is stable, and decide whether or not to run their own processing stages based on that information.

## Description

### Basic Theory

The total energy of the system of $M$ molecules (comprising $N$ atoms) is given by

$$ E_{total} = E_{geometry} + E_{pairpotential} $$

where

$$ E_{geometry} = \sum^M_{m} \left( \sum^{bonds}_b E^b_{ij} + \sum^{angles}_a E^a_{ijk} + \sum^{torsions}_t E^t_{ijkl} + \sum^{impropers}_i E^i_{ijkl} \right) $$

$$ E_{pairpotential} = \sum^N_{i,j \ge i} E^{LJ}_{ij} + \frac{q_i q_j}{r} $$

While $E_{geometry}$ is calculate analytically using the relevant functional forms of the interactions, $E_{pairpotential}$ always uses a tabulated potential for the short-range Lennard-Jones part of the interaction. Depending on how Dissolve is set up, this tabulated potential may also include the associated Coulomb interactions.

$E_{pairpotential}$ is calculated using standard rules for interactions occurring within the same molecule - 1,2 and 1,3 interactions (i.e. those between atoms at either end of defined bonds or angles) are excluded completely, while 1,4 interactions (i.e. between atoms at either ends of torsion interactions) are scaled by a factor of one half.

### Interaction Forms

Dissolve implements several functional forms for each of the distinct intra- and interatomic interaction types, and which are detailed in the [functional forms]({{< ref "forms" >}}) section.

### Stability

The stability of the total energy is an important thing to consider when running simulations - typically there is an equilibration or relaxation of the system at the beginning of any simulation, during which the energy changes significantly from step to step. During these periods where there are such large changes in the total energy it is unwise to run certain other calculations - for instance, one may not wish to have [molecular dynamics]({{< ref "md" >}}) kick in while the system is far from equilibrium, as this may lead to extreme forces on the atoms and potentially break your simulation. Dissolve therefore monitors the gradient of the energy and, along with a controlling threshold factor, decides whether the energy is stable (enough).

The `StabilityThreshold` value is multiplied by the mean of the last N points (governed by the `StabilityWindow` size) to give an absolute threshold energy value. The absolute value of the gradient over the last N points is compared to this value and, if it is lower, the energy is considered stable.

## Options

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Configuration`|`Configuration`|--|{{< required-label >}}Target configuration on which to operate.|

### Stability Detection

|Keyword|Arguments|Default|Description|
|:------|:-------:|:-----:|-----------|
|`StabilityThreshold`|`double`|`1.0e-3`|Threshold value at which energy is deemed stable over the defined windowing period|
|`StabilityWindow`|`int`|`10`|Number of points over which to assess the stability of the energy (per Configuration)|

## Export

|Keyword|Arguments|Default|Description|
|:------|:-------:|:-----:|-----------|
|`Save`|`bool`|`false`|Save calculated energies to disk, one file per targetted configuration|

## Advanced

|Keyword|Arguments|Default|Description|
|:------|:-------:|:-----:|-----------|
|`Test`|`bool`|`false`|Test parallel energy routines against basic serial versions and supplied reference values|
|`TestThreshold`|`double`|`0.1`|Threshold of energy at which test comparison will fail|
