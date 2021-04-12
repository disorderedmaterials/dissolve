---
title: XRaySQ (Module)
linkTitle: XRaySQ
description: Calculate x-ray-weighted S(Q) and G(r)
---

## Overview

`XRaySQ` is responsible for taking a set of previously calculated $S(Q)$ from an [`SQ`]({{< ref "sq" >}}) module and generating a new set of x-ray-weighted structure factors. The total x-ray-weighted structure factor, $F^{X}(Q)$, is generated through summation of the individual partial x-ray-weighted $S(Q)$. X-ray-weighted partial and total radial distribution functions are also calculated.

The `XRaySQ` module does not target any configurations itself - the underlying [`RDF`]({{< ref "rdf" >}}) module, referenced by the [`SQ`]({{< ref "sq" >}}) module, dictates the source configuration data.

## Description

### Basic Theory

The `XRaySQ` module calculates the Faber-Ziman partial structure factors weighted by the atomic x-ray scattering lengths, such that

$$ S^{X}_{ij}(Q) = b_i(Q) b_j(Q) S\_{ij}(Q) $$

where $b_i(Q)$ and $b_j(Q)$ are the Q-dependent form factors of the atom types $i$ and $j$ respectively. In a similar manner, the total x-ray-weighted structure factor is

$$ F^{X}(Q) = \sum^N_{i,j,i \geq j} [2-\delta_{ij}] b_i(Q) b_j(Q) c_i c_j S_{ij}(Q) $$

Depending on the need, $F^{X}(Q)$ may be normalised to an appropriate constant with the [`Normalisation`](#control-keywords) keyword.

### Instrumental Broadening

The application of instrumental broadening is the responsibility of the source [`SQ`]({{< ref "sq" >}}) module - see its [`QBroadening`]({{< ref "sq#control-keywords" >}}) keyword.

## Configuration

### Control Keywords
|Keyword|Arguments|Default|Description|
|:------|:-------:|:-----:|-----------|
|`FormFactors`|[`XRayFormFactors`]({{< ref "xrayformfactors" >}})|`WK1995`|Atomic form factors to use for weighting|
|`Normalisation`|[`NormalisationType`]({{< ref "normalisationtype" >}})|`None`|Quantity against which to normalise the total structure factor|
|`SourceSQs`|`Module`|--|{{< required-label >}} Source [`SQ`]({{< ref "sq" >}}) module from which to take unweighted $S(Q)$.|

### Reference Data
Keyword|Arguments|Default|Description|
|:------|:-------:|:-----:|-----------|
|`Reference`|[`Data1DFileAndFormat`]({{< ref "data1dformat" >}})|--|Format and filename of reference $F(Q)$ data, to be displayed in the GUI alongside calculated data, and made available for other modules to utilise (e.g. [`EPSR`]({{< ref "epsr" >}})|
|`ReferenceNormalisation`|[`NormalisationType`]({{< ref "normalisationtype" >}})|`None`|Assumed normalisation type to remove from the reference total structure factor data once loaded|
|`ReferenceWindowFunction`|[`WindowFunction`]({{< ref "broadeningfunction" >}})|`Lorch0`|Window function to apply when Fourier-transforming reference $F(Q)$ to a reference $g(r)$|

### Export
|Keyword|Arguments|Default|Description|
|:------|:-------:|:-----:|-----------|
|`SaveFormFactors`|`true|false`|`false`|Save Q-dependent form factors for each atom type pair|
|`SaveGR`|`true|false`|`false`|Save weighted g(r) and G(r). Separate files are written for each partial between atom types $i$ and $j$, as well as the total.||
|`SaveReference`|`true|false`|`false`|Save the reference data and its Fourier transform|
|`SaveSQ`|`true|false`|`false`|Save weighted partial and total structure factors. Separate files are written for each partial between atom types $i$ and $j$, as well as the total.|
