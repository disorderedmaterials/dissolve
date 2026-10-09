---
title: QSpecies (Module)
linkTitle: QSpecies
description: Calculate the number of bridging oxygen sites bonded to a network former.
---

## Overview

The `QSpecies` module calculates the Qn distribution of a site, where n is the number of bridging oxygen sites within the distance range specified. It also calculates the frequency of bridging, non-bridging and free oxygen sites.

{{< cimage src="qspecies.png" caption="Representation of Qn species." >}}

A note about how the distance range affects the calculation - the range specified here affects, as one would expect, the allowed distances between network former and bonding oxygen sites. However, this implicitly also affects which oxygen sites are considered as bonding / bridging oxygens. Increasing the distance range means that oxygen atoms will "see" more network former sites, but an oxygen must be near to exactly two network former sites in order to be considered a bridging oxygen. Thus, increasing the cutoff beyond a reasonable distance will not generate higher-order Qn species as one might expect. The trend will instead be towards Q0 as the number of bridging oxygen sites will also tend to zero.

## Options

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Configuration`|`Configuration`|--|{{< required-label >}}Specifies the configuration on which to operate.|

### Sites

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`BondingOxygen`|`Site ...`|--|Set the site(s) 'BO' which are to represent the bonding oxygen.|
|`NetworkFormer`|`Site ...`|--|Set the site(s) 'NF' for which the distribution around the origin sites 'SiteA' should be calculated.|

### Ranges

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`DistanceRange`|`double`<br/>`double`<br/>`double`|`0.0`<br/>`2.5`<br/>`0.05`|Distance range (min, max) over which to calculate Q-species from central site.|

## Export

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Export`|[`Data1DFileAndFormat`]({{< ref "data1dformat" >}})|--|File format and file name under which to save calculated Q-species data.|
