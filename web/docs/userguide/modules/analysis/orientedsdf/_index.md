---
title: OrientedSDF (Module)
linkTitle: OrientedSDF
description: Calculate a three-dimensional spatial distribution function, restricted by axis angles
---

## Overview

The `OrientedSDF` module calculates a three-dimensional spatial density / distribution function between two sites A and B in a manner analogous to the {{< module "SDF" >}} module, but restricting the selected sites of B based on an angle range between two site axis vectors. Thus, the generated function represents a spatial density of B sites with a particular orientation relative to A. As such, both the central site A and target site B must be oriented sites with full axis definitions.

## Options

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Configuration`|`Configuration`|--|{{< required-label >}}Specifies the configuration on which to operate.|

### Sites

|Keyword|Arguments|Default| Description                                                                                |
|:------|:--:|:-----:|--------------------------------------------------------------------------------------------|
|`SiteA`|`Site ...`|--| Specify site(s) which represent the central site 'A'. All specified sites must be oriented.|
|`AxisA`|[`SiteAxis`]({{< ref "siteaxis" >}})|`XAxis`|Axis representing the vector on site A to use in forming the angle.|
|`SiteB`|`Site ...`|--| Specify site(s) which represent the surrounding site 'B'. All specified sites must be oriented.|
|`AxisB`|[`SiteAxis`]({{< ref "siteaxis" >}})|`XAxis`|Axis representing the vector on site B to use in forming the angle.|

### Ranges

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`AngleRange`|`double`<br/>`double`|`-15.0`<br/>`15.0`|Axis angle range to accept when binning site B|
|`RangeX`|`double`<br/>`double`<br/>`double`|`-10.0`<br/>`10.0`<br/>`0.5`|Range (min, max, bin width) of volume over which to bin along X.|
|`RangeY`|`double`<br/>`double`<br/>`double`|`-10.0`<br/>`10.0`<br/>`0.5`|Range (min, max, bin width) of volume over which to bin along Y.|
|`RangeZ`|`double`<br/>`double`<br/>`double`|`-10.0`<br/>`10.0`<br/>`0.5`|Range (min, max, bin width) of volume over which to bin along Z.|

### Exclusions & Restrictions

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`ExcludeSameMolecule`|`bool`|`false`| Whether to exclude correlations between A and B sites on the same molecule.|
|`Symmetric`|`bool`|`false`| Whether to consider the angle as symmetric about 90, mapping all angles to the 0-90 range. For instance, if the target axes are perpendicular to a mirror plane in the molecule, this should be set to `true`.|

## Export

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`ExportSDF`|[`Data3DFileAndFormat`]({{< ref "data3dformat" >}})|--|File format and file name under which to save calculated SDF data.|
