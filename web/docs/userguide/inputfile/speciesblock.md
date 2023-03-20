---
title: Species Block
description: Defines a single molecular/atomic species
---

## Overview

The `Species` block completely defines an atomic or molecular species for use in a simulation. This description includes:the atomic contents, their connectivity

The block keyword itself takes a single (required) argument - the name of the species. This name is used to reference the species when, for example, constructing a configuration.

For more information see the section on [setting up species]({{< ref "species" >}}).

## Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Angle`|`i`<br/>`j`<br/>`k`<br/>`form`<br/>[`parameters`]|--|Create an intramolecular angle definition between the three atom indices `i`-`j`-`k`, and with the named [functional `form`]({{< ref "angle" >}}) and `parameters`. The [functional form]({{< ref "angle" >}}) may instead be replaced with the name of an existing [master term]({{< ref "masterblock" >}}) preceded with an ampersand (`@`), in which case the master term form and parameters will be used for this interaction.|
|`Atom`|`index`<br/>`element`<br/>`x`<br/>`y`<br/>`z`<br/>`type`<br/>[`q`]|--|Creates a new atom with integer `index` and chemical `element` at the `x`, `y`, and `z` coordinates specified, and with the associated atom `type` (or `None`). Optionally, the atomic charge `q` may be provided if required. Alternatively, the atomic charge may be specified for the atom with the `Charge` keyword.|
|`Bond`|`i`<br/>`j`<br/>`form`<br/>[`parameters`]|--|Create an intramolecular bond definition between the two atom indices `i`-`j`, and with the named [functional `form`]({{< ref "bond" >}}) and `parameters`. The [functional form]({{< ref "bond" >}}) may instead be replaced with the name of an existing [master term]({{< ref "masterblock" >}}) preceded with an ampersand (`@`), in which case the master term form and parameters will be used for this interaction.|
|`BondType`|`i`<br/>`j`<br/>`type`<br/>|--|Explicitly sets the bond type between the atoms `i` and `j` to be of the specified [`type`]({{< ref "bondtype" >}}). The bond must already exist.|
|`Charge`|`i`<br/>`q`|--|Sets the atomic charge of the atom with index `i` to `q`. Alternatively, the atomic charge may be specified as the last argument to the `Atom` keyword.|
|`CoordinateSets`|[`CoordinatesFileAndFormat`]({{< ref "coordinatesformat" >}})|--|Sets a file from which coordinates for the species are read in, and which are used when adding multiple copies of the species to a [configuration]({{< ref "configurationblock" >}}) in its generator definition.|
|`EndSpecies`||--|Indicates the end of the current `Species` block.|
|`Forcefield`|`name`|--|Applies inter- and intramolecular terms from the `name`d forcefield to the Species, overwriting any that previously exist.|
|`Improper`|`i`<br/>`j`<br/>`k`<br/>`l`<br/>`form`<br/>[`parameters`]|--|Create an intramolecular improper torsion definition between the four atom indices `i`-`j`-`k`-`l` (where `i` is the central atom), and with the named [functional `form`]({{< ref "improper" >}}) and `parameters`. The [functional form]({{< ref "improper" >}}) may instead be replaced with the name of an existing [master term]({{< ref "masterblock" >}}) preceded with an ampersand (`@`), in which case the master term form and parameters will be used for this interaction.|
|`Isotopologue`|`name`<br/>`type=Z ...`|--|Create an isotopologue called `name`, with isotopes assigned to atom `type`s by their `Z` value.|
|`Site`|`name`|--|Begin a [`site block`]({{< ref "siteblock" >}}) defining a reference site on the species in terms of its atoms for use in the [analysis modules]({{< ref "analysis" >}}).|
|`Torsion`|`i`<br/>`j`<br/>`k`<br/>`l`<br/>`form`<br/>[`parameters`]|--|Create an intramolecular torsion (dihedral angle) definition between the four atom indices `i`-`j`-`k`-`l`, and with the named [functional `form`]({{< ref "torsion" >}}) and `parameters`. The [functional form]({{< ref "torsion" >}}) may instead be replaced with the name of an existing [master term]({{< ref "masterblock" >}}) preceded with an ampersand (`@`), in which case the master term form and parameters will be used for this interaction.|

## Examples

### Water

```r
Species  'Water'
  Atom    1    O     5.139     5.968     0.000  'OW'    -0.82
  Atom    2    H     3.924     5.424     0.000  'HW'     0.41
  Atom    3    H     6.088     5.120     0.000  'HW'     0.41

  Bond    1    2  Harmonic  4431.530     0.976
  Bond    3    1  Harmonic  4431.530     0.976

  Angle    3    1    2  Harmonic   317.566   107.134

  Isotopologue  'Deuterated'  HW=2

  Site  'COM'
    Origin  1  2  3
    OriginMassWeighted  True
    XAxis  1
    YAxis  3
  EndSite

EndSpecies
```

### Water (with master terms)

```r
Master
  Bond  'HW-OW'  Harmonic  4431.530     0.976
  Angle  'HW-OW-HW'  Harmonic   317.566   107.134
EndMaster

Species  'Water'
  Atom    1    O     5.139     5.968     0.000  'OW'    -0.82
  Atom    2    H     3.924     5.424     0.000  'HW'     0.41
  Atom    3    H     6.088     5.120     0.000  'HW'     0.41

  Bond    1    2  @HW-OW
  Bond    3    1  @HW-OW

  Angle    3    1    2  @HW-OW-HW

  ...

EndSpecies
```
