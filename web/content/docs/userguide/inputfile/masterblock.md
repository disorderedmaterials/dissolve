---
title: Master Block
description: Define frequently-used intramolecular parameters for species
---

## Overview

The `Master` block contains definitions of named intramolecular terms (bonds, angles, torsions, and  impropers) which are referenced by one or more defined [`Species`]({{< ref "speciesblock" >}}) when describing their own intramolecular interactions. By utilising master terms repetition of identical intramolecular terms within species definitions can be avoided. In addition, if parameters within intramolecular interactions need to be edited, using master terms reduces editing to single terms within the `Master` block, rather than editing multiple terms throughout the species. Multiple `Master` blocks may be defined if required.

Species may reference defined master terms by giving the name of the master term preceded by an `@` instead of the functional form of the interaction. This is illustrated in the example below.
## Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Angle`|`name`<br/>[`AngleFunctionalForm`]({{< ref "angle" >}})<br/>`params...`|--|Defines an angle master term with the specified `name` and [functional form]({{< ref "angle" >}}), with the parameters given in the order required by that particular [functional form]({{< ref "angle" >}}).|
|`Bond`|`name`<br/>[`BondFunctionalForm`]({{< ref "bond" >}})<br/>`params...`|--|Defines a bond master term with the specified `name` and [functional form]({{< ref "bond" >}}), with the parameters given in the order required by that particular [functional form]({{< ref "bond" >}}).
|`EndMaster`|--|--|Indicates the end of the current `Master` block.|
|`Improper`|`name`<br/>[`ImproperFunctionalForm`]({{< ref "improper" >}})<br/>`params...`|--|Defines an improper torsion master term with the specified `name` and [functional form]({{< ref "improper" >}}), with the parameters given in the order required by that particular [functional form]({{< ref "improper" >}}).
|`Torsion`|`name`<br/>[`TorsionFunctionalForm`]({{< ref "torsion" >}})<br/>`params...`|--|Defines a torsion master term with the specified `name` and [functional form]({{< ref "torsion" >}}), with the parameters given in the order required by that particular [functional form]({{< ref "torsion" >}}).

## Example

```r
Master
  Bond  'HW-OW'  Harmonic  4431.530     0.976
  Angle  'HW-OW-HW'  Harmonic   317.566   107.134
EndMaster

Species 'Water'
  Atom    1    O     5.139     5.968     0.000  'OW'    -0.82
  Atom    2    H     3.924     5.424     0.000  'HW'     0.41
  Atom    3    H     6.088     5.120     0.000  'HW'     0.41

  Bond    1    2  @HW-OW
  Bond    3    1  @HW-OW

  Angle    3    1    2  @HW-OW-HW
EndSpecies
```
