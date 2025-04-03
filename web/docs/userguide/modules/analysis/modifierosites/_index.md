---
title: ModifierOSites (Module)
linkTitle: ModifierOSites
description: Calculate the bonding type and total bonding to oxygen in reference to a network former.
---

## Overview

The `ModifierOSites` module calculates how a modifier is bonding into a network. The frequency of bonding to bridging, non-bridging and free oxygen sites, in reference to a network former, is calculated. The total bonds to oygen as well as the bond lengths to oxygen are also calculated.

## Options

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Configuration`|`Configuration`|--|{{< required-label >}}Specifies the configuration on which to operate.|

### Sites

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`BondingOxygen`|`Site ...`|--|Set the site(s) 'BO' which are to represent the bonding oxygen.|
|`NetworkFormer`|`Site ...`|--|Set the site(s) 'NF' for which the distribution around the origin sites 'A' should be calculated.|
|`SiteA`|`Site ...`|--|Set the modifier site(s) for which the distribution of oxygens should be calculated.|


### Ranges

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`DistanceRange`|`double`<br/>`double`<br/>`double`|`0.0`<br/>`2.5`<br/>`0.05`|Distance range (min, max) over which to calculate Q-Species from central site.|
|`ModifierDistanceRange`|`double`<br/>`double`<br/>`double`|`0.0`<br/>`2.0`<br/>`0.05`|Distance range (min, max) over which to calculate Modifier from central site.|

## Export

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`ExportModifierData`|[`Data1DFileAndFormat`]({{< ref "data1dformat" >}})|--|File format and file name under which to save calculated modifier to oxygen data.|
|`ExportOxygenTypes`|[`Data1DFileAndFormat`]({{< ref "data1dformat" >}})|--|File format and file name under which to save calculated total oxygens to modifier atoms.|
|`ExportFOLength`|[`Data1DFileAndFormat`]({{< ref "data1dformat" >}})|--|File format and file name under which to save calculated free oxygens to modifier atom, bond length.|
|`ExportNBOLength`|[`Data1DFileAndFormat`]({{< ref "data1dformat" >}})|--|File format and file name under which to save calculated non-bridging oxygens to modifier atom, bond length.|
|`ExportBOLength`|[`Data1DFileAndFormat`]({{< ref "data1dformat" >}})|--|File format and file name under which to save calculated bridging oxygens to modifier atom, bond length.|
|`ExportOtherOLength`|[`Data1DFileAndFormat`]({{< ref "data1dformat" >}})|--|File format and file name under which to save calculated other oxygens to modifier atom, bond length.|
