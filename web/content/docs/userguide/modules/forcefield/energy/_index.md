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

## Configuration

### Control Keywords
|Keyword|Arguments|Default|Description|
|:------|:-------:|:-----:|-----------|
|`StabilityThreshold`|`double`|`1.0e-3`|Threshold value at which energy is deemed stable over the defined windowing period|
|`StabilityWindow`|`int`|`10`|Number of points over which to assess the stability of the energy (per Configuration)|

### Test Keywords
|Keyword|Arguments|Default|Description|
|:------|:-------:|:-----:|-----------|
|`Test`|`true|false`|`false`|Test parallel energy routines against basic serial versions and supplied reference values|
|`TestAnalytic`|`true|false`|`false`|Use analytic interatomic energies rather than (production) tabulated potentials for tests|
|`TestReferenceInter`|`double`|`0.0`|Reference value for interatomic energy against which to test calculated value|
|`TestReferenceIntra`|`double`|`0.0`|Reference value for intramolecular energy against which to test calculated value|
|`TestThreshold`|`double`|`0.1`|Threshold of energy at which test comparison will fail|

### Export Keywords
|Keyword|Arguments|Default|Description|
|:------|:-------:|:-----:|-----------|
|`Save`|`true|false`|`false`|Save calculated energies to disk, one file per targetted configuration|
