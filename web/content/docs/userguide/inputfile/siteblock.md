---
title: Site Block
description: Specifies a reference site on a species for use by calculation / analysis routines
---

## Overview

The `Site` block defines a named analysis site on a [`Species`]({{< ref "speciesblock" >}}) that can later be referenced in one of the [analysis modules]({{< ref "analysis" >}}).

`Site` blocks must always be defined within the [`Species`]({{< ref "speciesblock" >}}) to which they relate. Multiple `Site` blocks can be defined within a single [`Species`]({{< ref "speciesblock" >}}).

## Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`EndSite`|`int`|--|Indicates the end of the current `Site` block.|
|`Origin`|`int`<br/>`[int...]`|--|Define the origin of the site to be the geometric mean of the coordinates of the atom(s) with the indices provided.|
|`OriginMassWeighted`|`int`<br/>`[int...]`|--|Define the origin of the site to be the mass-weighted average of the coordinates of the atom(s) with the indices provided.|
|`XAxis`|`int`<br/>`[int...]`|--|Define the reference point indicating the x-axis of the site to be the geometric mean of the coordinates of the atom(s) with the indices provided.|
|`YAxis`|`int`<br/>`[int...]`|--|Define the reference point indicating the y-axis of the site to be the geometric mean of the coordinates of the atom(s) with the indices provided.|

## Example

### Methanol

Given the species atoms as:

```r
Species 'Methanol'
  # Atoms
  Atom    1    C  3.413690e-01  1.199640e-01  -4.430163e-02  'CT'  1.450000e-01
  Atom    2    O  -9.265791e-01  -3.425229e-01  -4.577432e-01  'OH'  -6.830000e-01
  Atom    3    H  -1.468049e+00  -4.997640e-01  3.024601e-01  'HO'  4.180000e-01
  Atom    4    H  2.750356e-01  1.114836e+00  3.749080e-01  'HC'  4.000000e-02
  Atom    5    H  1.027597e+00  1.519417e-01  -8.758279e-01  'HC'  4.000000e-02
  Atom    6    H  7.506264e-01  -5.444546e-01  7.005046e-01  'HC'  4.000000e-02

  ...
EndSpecies
```

We could define the following sites:

#### Hydroxyl Proton

Centered on the alcohol proton, with no defined axes.

```r
Site  'C(OH)'
  Origin  3
EndSite
```

#### Centre of Methyl Group

Origin located at the geometric centre of the three methyl hydrogens.

```r
Site  'C(OH)'
  Origin  4 5 6
EndSite
```

#### Hydroxyl Oxygen (with axes)

Centred on the oxygen, and using the carbon to define the absolute direction of the x-axis, and the alcohol proton to define the implied direction of the y-axis.

```r
Site  'C(OH)'
  Origin  2
  XAxis  1
  YAxis  3
EndSite
```
