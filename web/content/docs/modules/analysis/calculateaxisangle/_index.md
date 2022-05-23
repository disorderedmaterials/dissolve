---
title: CalculateAxisAngle (Module)
linkTitle: CalculateAxisAngle
description: Calculate distance distribution and angle map between oriented sites A and B for A&middot;&middot;&middot;B&middot;&middot;&middot;C
---

## Overview

The `CalculateAxisAngle` module calculates the angle between two vectors $u$ and $v$ from two oriented sites A and B, forming the averages of the angle, the A&middot;&middot;&middot;B distances, and the distance-angle map.

## Keywords

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Configuration`|`Configuration`|`--`|{{< required-label >}}Specifies the configuration on which to operate.|

## Control

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`AngleRange`|`double`<br/>`double`<br/>`double`|`0.0`<br/>`180.0`<br/>`10.0`|Range (min, max, binwidth) of angle binning.|
|`AxisA`|[`SiteAxis`]({{< ref "siteaxis" >}})|`XAxis`|Axis representing the vector on site A to use in forming the angle.|
|`AxisB`|[`SiteAxis`]({{< ref "siteaxis" >}})|`XAxis`|Axis representing the vector on site B to use in forming the angle.|
|`DistanceRange`|`double`<br/>`double`<br/>`double`|`0.0`<br/>`10.0`<br/>`0.05`|Range (min, max, binwidth) of A-B distance binning.|
|`ExcludeSameMolecule`|`bool`|false`|Whether to exclude correlations between A and B sites on the same molecule.|
|`SiteA`|`name ...`|--|Specify site(s) which represent the first site 'A'.|
|`SiteB`|`name ...`|--|Specify site(s) which represent the second site 'B'.|
|`Symmetric`|`bool`|`false`|Whether to consider the angle as symmetric about 90, mapping all angles to the 0-90 range.|

## Export

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`ExportAngle`|[`Data1DFileAndFormat`]({{< ref "data1dformat" >}})|--|File format and file name under which to save calculated angle histogram.|
|`ExportDAngle`|[`Data2DFileAndFormat`]({{< ref "data2dformat" >}})|--|File format and file name under which to save calculated distance-angle map.|
|`ExportRDF`|[`Data1DFileAndFormat`]({{< ref "data1dformat" >}})|--|File format and file name under which to save calculated A-B RDF data.|
