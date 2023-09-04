---
title: SiteRDF (Module)
linkTitle: SiteRDF
description: Calculate a site-site radial distribution function and associated coordination numbers
---

## Overview

The `SiteRDF` module calculates a radial distribution function between two sites A and B over a defined distance range. Optionally up to three coordination numbers can be calculated over defined ranges of the resulting RDF.

## Options

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Configuration`|`Configuration`|--|{{< required-label >}}Specifies the configuration on which to operate.|

### Control

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|

### Sites

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`SiteA`|`Site ...`|--|Specify site(s) which represent 'A' in the interaction A-B.|
|`SiteB`|`Site ...`|--|Specify site(s) which represent 'B' in the interaction A-B.|

### Ranges

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`DistanceRange`|`double`<br/>`double`<br/>`double`|`0.0`<br/>`10.0`<br/>`0.05`|Range (min, max, bin width) of A-B distance binning.|

### Control

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`ExcludeSameMolecule`|`bool`|`false`|Whether to exclude correlations between A and B sites on the same molecule.|
|`Instantaneous`|`bool`|`false`|Whether to calculate only the instantaneous coordination numbers, rather than the running average.|

### Coordination Numbers

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`RangeA`|`double`<br/>`double`|`0.0`<br/>`3.0`|Specifies the distance range over which to calculate the first coordination number.|
|`RangeAEnabled`|`bool`|`false`|Whether calculation of the first coordination number is enabled.|
|`RangeB`|`double`<br/>`double`|`3.0`<br/>`6.0`|Specifies the distance range over which to calculate the second coordination number.|
|`RangeBEnabled`|`bool`|`false`|Whether calculation of the second coordination number is enabled.|
|`RangeC`|`double`<br/>`double`|`6.0`<br/>`9.0`|Specifies the distance range over which to calculate the third coordination number.|
|`RangeCEnabled`|`bool`|`false`|Whether calculation of the third coordination number is enabled.|

## Export

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Export`|[`Data1DFileAndFormat`]({{< ref "data1dformat" >}})|--|File format and file name under which to save calculated A-B RDF data.|
|`ExportInstantaneousCN`|`bool`|`false`|Whether to export instantaneous coordination numbers to disk.|

