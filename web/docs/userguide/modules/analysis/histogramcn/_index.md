---
title: HistogramCN (Module)
linkTitle: HistogramCN
description: Calculate a histogram of coordination numbers for two sites
---

## Overview

The `HistogramCN` module calculates a distribution of coordination numbers between two sites A and B over a defined distance range.

## Options

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Configuration`|`Configuration`|--|{{< required-label >}}Specifies the configuration on which to operate.|

### Sites

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`SiteA`|`Site ...`|--|Set the site(s) 'A' which are to represent the reference origin.|
|`SiteB`|`Site ...`|--|Set the site(s) 'B' for which the coordination number around the origin sites should be calculated.|

### Ranges

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`DistanceRange`|`double`<br/>`double`<br/>`double`|`0.0`<br/>`5.0`<br/>`0.05`|Distance range (min, max) over which to calculate coordination number from central site.|

## Export

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Export`|[`Data1DFileAndFormat`]({{< ref "data1dformat" >}})|--|File format and file name under which to save calculated HitogramCN data.|
