---
title: SDF (Module)
linkTitle: SDF
description: Calculate a three-dimensional spatial distribution function
---

## Overview

The `SDF` module calculates a three-dimensional spatial density / distribution function between two sites A and B over a defined volume range. The central site A must be an oriented site.

See also the {{< module "OrientedSDF" >}} module.

## Options

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Configuration`|`Configuration`|--|{{< required-label >}}Specifies the configuration on which to operate.|

### Sites

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`SiteA`|`Site ...`|--|Specify site(s) which represent the central site 'A'. All specified sites must be oriented.|
|`SiteB`|`Site ...`|--|Specify site(s) which represent the surrounding site 'B'.|

### Ranges

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`RangeX`|`double`<br/>`double`<br/>`double`|`-10.0`<br/>`10.0`<br/>`0.5`|Range (min, max, bin width) of volume over which to bin along X.|
|`RangeY`|`double`<br/>`double`<br/>`double`|`-10.0`<br/>`10.0`<br/>`0.5`|Range (min, max, bin width) of volume over which to bin along Y.|
|`RangeZ`|`double`<br/>`double`<br/>`double`|`-10.0`<br/>`10.0`<br/>`0.5`|Range (min, max, bin width) of volume over which to bin along Z.|

### Exclusions

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`ExcludeSameMolecule`|`bool`|`false`|Whether to exclude correlations between A and B sites on the same molecule.|

## Export

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`ExportSDF`|[`Data3DFileAndFormat`]({{< ref "data3dformat" >}})|--|File format and file name under which to save calculated SDF data.|
