---
title: Forces (Module)
linkTitle: Forces
description: Calculate atomic forces for configurations
---

## Overview

The `Forces` module calculates the total atomic forces present in one or more configurations arising from intramolecular and intermolecular interactions between atoms. It is not normally required to be present in a typical simulation - modules such as [`MD`]({{< ref "md" >}}) directly call the necessary force functions, for instance - but it can be used to test against reference data in order to ensure consistency with other simulation codes.

## Description

### Basic Theory

The total forces occurring within a system of $M$ molecules (comprising $N$ atoms) is given by

$$ F_{total} = F_{geometry} + F_{pairpotential} $$

where

$$ F_{geometry} = \sum^M_{m} \left( \sum^{bonds}_b F^b_{ij} + \sum^{angles}_a F^a_{ijk} + \sum^{torsions}_t F^t_{ijkl} + \sum^{impropers}_i F^i_{ijkl} \right) $$

$$ F_{pairpotential} = \sum^N_{i,j \ge i} F^{LJ}_{ij} + \frac{q_i q_j}{r} $$

While $F_{geometry}$ is calculate analytically using the relevant functional forms of the interactions, $F_{pairpotential}$ always uses a tabulated force for the short-range Lennard-Jones part of the interaction. Depending on how Dissolve is set up, this tabulated potential may also include the forces arising from the associated Coulomb interactions.

$F_{pairpotential}$ is calculated using standard rules for interactions occurring within the same molecule - 1,2 and 1,3 interactions (i.e. those between atoms at either end of defined bonds or angles) are excluded completely, while 1,4 interactions (i.e. between atoms at either ends of torsion interactions) are scaled by a factor of one half.

### Interaction Forms

Dissolve implements several functional forms for each of the distinct intra- and interatomic interaction types, and which are detailed in the [functional forms]({{< ref "forms" >}}) section.

## Configuration

### Test Keywords
|Keyword|Arguments|Default|Description|
|:------|:-------:|:-----:|-----------|
|`Test`|`true|false`|`false`|Test parallel force routines against basic serial versions and supplied reference values (if provided)|
|`TestAnalytic`|`true|false`|`false`|Use analytic interatomic energies rather than (production) tabulated potentials for tests|
|`TestInter`|`true|false`|`true`|Include interatomic forces in test|
|`TestIntra`|`true|false`|`true`|Include intramolecular forces in test|
|`TestReference`|[`ForcesFileAndFormat`]({{< ref "forcesformat" >}})|--|Reference forces to test calculated forces against|
|`TestThreshold`|`double`|`0.1`|Threshold of force (%) at which test comparison will fail|

### Export Keywords
|Keyword|Arguments|Default|Description|
|:------|:-------:|:-----:|-----------|
|`SaveForces`|[`ForcesFileAndFormat`]({{< ref "forcesformat" >}})|--|Save calculated energies to the specified file / format|
