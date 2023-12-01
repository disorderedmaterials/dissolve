---
title: DAngle (Module)
linkTitle: DAngle
description: Calculate distance distribution and angle map for A&ndash;B&middot;&middot;&middot;C
---

## Overview

The `DAngle` module calculates the angle between two vectors $u$ and $v$ from two oriented sites A and B, forming the averages of the angle, the A&middot;&middot;&middot;B distances, and the distance-angle map. The sites A and B must be present on the same molecule - C may exist anywhere. Internal angles - i.e. between sites A-B-C all on the same molecule - are excluded by default from the calculation, but may be included if required. If more flexibility is required, see the {{< module "Angle" >}} module.

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
|`AngleRange`|`double`<br/>`double`<br/>`double`|`0.0`<br/>`180.0`<br/>`10.0`|Range (min, max, bin width) of angle binning.|
|`DistanceRange`|`double`<br/>`double`<br/>`double`|`0.0`<br/>`10.0`<br/>`0.05`|Range (min, max, bin width) of A-B distance binning.|

### Exclusions & Restrictions

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`ExcludeSameMolecule`|`bool`|false`|Whether to exclude correlations between B and C sites on the same molecule.|

## Export

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`ExportAngle`|[`Data1DFileAndFormat`]({{< ref "data1dformat" >}})|--|File format and file name under which to save calculated A-B-C angle histogram.|
|`ExportDAngle`|[`Data2DFileAndFormat`]({{< ref "data2dformat" >}})|--|File format and file name under which to save calculated (A-B)-C distance-angle map.|
|`ExportRDF`|[`Data1DFileAndFormat`]({{< ref "data1dformat" >}})|--|File format and file name under which to save calculated A-B RDF data.|
