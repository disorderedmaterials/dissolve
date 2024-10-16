---
title: NeutronSQ (Module)
linkTitle: NeutronSQ
description: Calculate neutron-weighted S(Q) and G(r)
---

## Overview

`NeutronSQ` is responsible for taking a set of previously calculated $S(Q)$ from an {{< module "SQ" >}} module and generating a new set of neutron-weighted structure factors. The total neutron-weighted structure factor, $F^{N}(Q)$, is generated through summation of the individual partial neutron-weighted $S(Q)$. Neutron-weighted partial and total radial distribution functions are also calculated.

The `NeutronSQ` module does not target any configurations itself - the underlying  {{< module "GR" >}}  module, referenced by the {{< module "SQ" >}} module, dictates the source configuration data.

## Description

### Basic Theory

The `NeutronSQ` module calculates the Faber-Ziman partial structure factors weighted by the coherent atomic neutron scattering lengths, such that

$$ S^{N}_{ij}(Q) = b_i b_j S\_{ij}(Q) $$

where $b_i$ and $b_j$ are the bound coherent scattering lengths of the atom types $i$ and $j$ respectively. In a similar manner, the total neutron-weighted structure factor is

$$ F^{N}(Q) = \sum^N_{i,j,i \geq j} [2-\delta_{ij}] b_i b_j c_i c_j S_{ij}(Q) $$

Depending on the need, $F^{N}(Q)$ may be normalised to an appropriate constant with the [`Normalisation`](#control) keyword.

### Weighting

The module depends on the definition of suitable isotopologues for each species of interest in order to determine the correct $b$ values to use, and which are specified with the [`Isotopologue`](#control) keyword. Furthermore, any atom types that undergo fast exchange (e.g. hydrogens attached to oxygen or nitrogen atoms) can be indicated with the [`Exchangeable`](#control) keyword.

With isotopologues and exchangeable atoms provided, matrices of weighting factors between atom types on the same and different molecules can be constructed. The reason to distinguish between intramolecular and intermolecular weightings is perhaps not immediately apparent, but is necessary to correctly calculate weighted $S(Q)$ for isotopic mixtures. Consider a simplified example where two molecules H-X-H and D-X-D, where the hydrogen and deuterium are **not** exchangeable, are mixed together in a 1:1 ratio. In a dynamic system such as a liquid, from the point of view of a molecule (H-X-H or D-X-D) the time-averaged picture of the surrounding molecules is a blend of indistinguishable H-X-H and D-X-D molecules. Averaging this picture over all molecules, the resulting _intermolecular_ weighting factor for the partial between the H/D atoms is a straightforward average of the two scattering lengths of H and D. However, locally within the molecule, an H is always next to another H, and a D is always next to another D. Thus, the _intramolecular_ weighting factor is an average of the squares of the scattering lengths of H and D. If the H and D sites are both exchangeable then the intramolecular weighting factor becomes the same as in the intermolecular case.

### Instrumental Broadening

The application of instrumental broadening is the responsibility of the source {{< module "SQ" >}} module - see its [`QBroadening`]({{< ref "sq#control" >}}) keyword.

## Options

### Targets
|Keyword|Arguments|Default|Description|
|:------|:-------:|:-----:|-----------|
|`SourceSQs`|`Module`|--|{{< required-label >}} Source {{< module "SQ" >}} module from which to take unweighted $S(Q)$.|

### Isotopes & Normalisation
|Keyword|Arguments|Default|Description|
|:------|:-------:|:-----:|-----------|
|`Exchangeable`|`AtomType` ...|--|A list of one or more atom types in the system that are exchangeable with each other|
|`Isotopologue`|`Configuration`</br>`Species`</br>`Isotopologue`</br>`double`|--|Set/add an isotopologue and its population for a particular species. The `Isotopologue` keyword may be used as many times as is necessary to satisfy the number of species in the system and their blends of isotopologues|
|`NormaliseTo`|[`NormalisationType`]({{< ref "normalisationtype" >}})|`None`|Normalisation to apply to the total weighted F(Q). The same normalisation is also applied to supplied reference data following removal (if appropriate) of its own normalisation (see the `ReferenceNormalisedTo` keyword)|


### Reference Data
Keyword|Arguments|Default|Description|
|:------|:-------:|:-----:|-----------|
|`Reference`|[`Data1DFileAndFormat`]({{< ref "data1dformat" >}})|--|Format and filename of reference $F(Q)$ data, to be displayed in the GUI alongside calculated data, and made available for other modules to utilise (e.g. {{< module "EPSR" >}})|
|`ReferenceFTDeltaR`|`double`|0.05|Spacing in $r$ to use when generating the Fourier-transform of the $F(Q)$|
|`ReferenceFTQMax`|`double`|--|Maximum Q value to use when Fourier-transforming the reference $F(Q)$ to its $G(r)$|
|`ReferenceFTQMin`|`double`|--|Minimum Q value to use when Fourier-transforming the reference $F(Q)$ to its $G(r)$|
|`ReferenceNormalisedTo`|[`NormalisationType`]({{< ref "normalisationtype" >}})|`None`|Normalisation which has been applied to the reference data|
|`ReferenceWindowFunction`|[`WindowFunction`]({{< ref "windowfunction" >}})|`Lorch0`|Window function to apply when Fourier-transforming reference $F(Q)$ to a reference $g(r)$|

## Export
|Keyword|Arguments|Default|Description|
|:------|:-------:|:-----:|-----------|
|`SaveGR`|`bool`|`false`|Save weighted g(r) and G(r). Separate files are written for each partial between atom types $i$ and $j$, as well as the total.||
|`SaveReference`|`bool`|`false`|Save the reference data and its Fourier transform|
|`SaveRepresentativeGR`|`bool`|`false`|Save the representative $G(r)$ obtained from Fourier transform of the calculated $F(Q)$|
|`SaveSQ`|`bool`|`false`|Save weighted partial and total structure factors. Separate files are written for each partial between atom types $i$ and $j$, as well as the total.|
