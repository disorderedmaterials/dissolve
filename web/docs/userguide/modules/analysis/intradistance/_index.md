---
title: IntraDistance (Module)
linkTitle: IntraDistance
description: Calculate a site-site distance histogram within molecules
---

## Overview

The `IntraDistance` module calculates a distance histogram between two sites A and B over a defined distance range. The function is calculated only between sites existing on the same molecule.

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

## Export

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Export`|[`Data1DFileAndFormat`]({{< ref "data1dformat" >}})|--|File format and file name under which to save calculated A-B histogram data.|

