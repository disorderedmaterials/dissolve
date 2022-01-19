---
title: PairPotentials Block
description: Defines interatomic pair potentials and cutoff information
---

## Overview

The `PairPotentials` block contains details of atom types and their associated interatomic potentials, and controls the range, binning, and truncation schemes for tabulated potentials generated from these parameters.

## Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`CoulombTruncation`|[`CoulombTruncationScheme`]({{< ref "coulombtruncationscheme" >}})|`Shifted`|Select the truncation scheme to use for coulomb interactions. This is a global parameter and applies to all pair potentials.|
|`Delta`|`double`|`0.005`|Spacing between points to use in the tabulated pair potential. This is a global parameter and applies to all pair potentials.|
|`EndPairPotentials`|||Indicates the end of the current `PairPotentials` block.||
|`ForceChargeSource`|`true|false`|`false`|Forces the user-specified choice of `IncludeCoulomb` to be used (if not letting Dissolve choose automatically).|
|`IncludeCoulomb`|`true|false`|`false`|When not letting Dissolve choose automatically, this determins whether Coulomb terms are included in the tabulated pair potentials. If `false` then the tabulated pair potentials contain only short-range contributions, with charge interactions calculated analytically from atomic charges defined in [`Species`]({{< ref "speciesblock" >}}). This is a global parameter and applies to all pair potentials.|
|`ManualChargeSource`|`true|false`|`false`|Whether the choice of pairpotential style (including charges from atom types, or taking charges from species atoms) is left to Dissolve (`false`) or the user (`true`) to decide.|
|`Parameters`|`Element`<br/>`name`<br/>`charge`<br/>[`ShortRangeForm`]({{< ref "short-range" >}})<br/>`params...`|--|Define a single atomtype called `name`, assignable to any atom of the specified `Element`, with atomic `charge` and short range form and parameters. Parameters must be given in the order expected by the specified [short range type]({{< ref "short-range" >}}). The atomic `charge` must always be supplied, but is only used if `IncludeCoulomb` is `true`.|
|`Range`|`double`|`15.0`|Maximum range of the pair potentials in the simulation. This is a global parameter and applies to all pair potentials.|
|`ShortRangeTruncation`|[`ShortRangeTruncationScheme`]({{< ref "shortrangetruncationscheme" >}})|`Shifted`|Select the truncation scheme to use for short-range interactions. This is a global parameter and applies to all pair potentials.|
|`ShortRangeTruncationWidth`|`double`|`2.0`|Truncation width to use for certain [short-range truncation schemes]({{< ref "shortrangetruncationscheme" >}}). This is a global parameter and applies to all pair potentials.|

## Example

```r
PairPotentials
  Parameters  Si  Si  2.000000e+00  LJ  0.175  1.03
  Parameters  O  O  -1.000000e+00  LJ  0.165  3.5
  Range  10.000000
  Delta  0.005000
EndPairPotentials

```
