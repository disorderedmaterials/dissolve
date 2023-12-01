---
title: Angle (Module)
linkTitle: Angle
description: Calculate distance distributions and angle maps for A&middot;&middot;&middot;B&middot;&middot;&middot;C
---

## Overview

The `Angle` module calculates the angle between three sites A&middot;&middot;&middot;B&middot;&middot;&middot;C, forming the averages of the angle, the A&middot;&middot;&middot;B and B&middot;&middot;&middot;C distances, and the distance-angle maps (A&middot;&middot;&middot;B)&middot;&middot;&middot;C and A&middot;&middot;&middot;(B&middot;&middot;&middot;C).

Unlike the {{< module "DAngle" >}} module, no restrictions are imposed on the relationship or connectivity between sites A, B, and C.

## Options

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Configuration`|`Configuration`|--|{{< required-label >}}Specifies the configuration on which to operate.|

### Sites

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`SiteA`|`Site ...`|--|Specify site(s) which represent 'A' in the interaction A-B-C.|
|`SiteB`|`Site ...`|--|Specify site(s) which represent 'B' in the interaction A-B-C.|
|`SiteC`|`Site ...`|--|Specify site(s) which represent 'C' in the interaction A-B-C.|

### Ranges

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`AngleRange`|`double`<br/>`double`<br/>`double`|`0.0`<br/>`180.0`<br/>`10.0`|Range (min, max, bin width) of A-B-C angle binning.|
|`RangeAB`|`double`<br/>`double`<br/>`double`|`0.0`<br/>`10.0`<br/>`0.05`|Range (min, max, bin width) of A-B distance binning.|
|`RangeBC`|`double`<br/>`double`<br/>`double`|`0.0`<br/>`10.0`<br/>`0.05`|Range (min, max, bin width) of B-C distance binning.|

### Exclusions & Restrictions

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`ExcludeSameMoleculeAB`|`bool`|`false`|Whether to exclude correlations between A and B sites on the same molecule.|
|`ExcludeSameMoleculeBC`|`bool`|`false`|Whether to exclude correlations between B and C sites on the same molecule.|
|`ExcludeSameSiteAC`|`bool`|`false`|Whether to exclude correlations between A and C sites on the same molecule.|
|`Symmetric`|`bool`|`false`|Whether to consider the angle A-B-C as symmetric about 90, mapping all angles to the 0-90 range.|

## Export

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`ExportAB`|[`Data1DFileAndFormat`]({{< ref "data1dformat" >}})|--|File format and file name under which to save calculated A-B RDF data.|
|`ExportAngle`|[`Data1DFileAndFormat`]({{< ref "data1dformat" >}})|--|File format and file name under which to save calculated A-B-C angle histogram.|
|`ExportBC`|[`Data1DFileAndFormat`]({{< ref "data1dformat" >}})|--|File format and file name under which to save calculated B-C RDF data.|
|`ExportDAngleAB`|[`Data2DFileAndFormat`]({{< ref "data2dformat" >}})|--|File format and file name under which to save calculated (A-B)-C distance-angle map.|
|`ExportDAngleBC`|[`Data2DFileAndFormat`]({{< ref "data2dformat" >}})|--|File format and file name under which to save calculated A-(B-C) distance-angle map.|
