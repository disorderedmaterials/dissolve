---
title: CalculateRDF (Module)
linkTitle: CalculateRDF
description: Calculate a site-site radial distribution function
---

## Overview

The `CalculateRDF` module calculates a radial distribution function between two sites A and B over a defined distance range.

## Keywords

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Configuration`|`Configuration`|`--`|{{< required-label >}}Specifies the configuration on which to operate.|

## Control

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`DistanceRange`|`double`<br/>`double`<br/>`double`|`0.0`<br/>`10.0`<br/>`0.05`|Range (min, max, bin width) of A-B distance binning.|
|`ExcludeSameMolecule`|`bool`|`false`|Whether to exclude correlations between A and B sites on the same molecule.|
|`SiteA`|`Site ...`|--|Specify site(s) which represent 'A' in the interaction A-B.|
|`SiteB`|`Site ...`|--|Specify site(s) which represent 'B' in the interaction A-B.|

## Export

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Export`|[`Data1DFileAndFormat`]({{< ref "data1dformat" >}})|--|File format and file name under which to save calculated A-B RDF data.|
