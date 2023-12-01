---
title: IntraAngle (Module)
linkTitle: IntraAngle
description: Calculate angle distributions within molecules
---

## Overview

The `IntraAngle` module calculates the angle histogram between three sites A&middot;&middot;&middot;B&middot;&middot;&middot;C. The angle is only calculated between sites present in the same molecule, and so represents an exclusively intramolecular angle distribution.

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

### Restrictions

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Symmetric`|`bool`|`false`|Whether to consider the angle A-B-C as symmetric about 90, mapping all angles to the 0-90 range.|

## Export

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`ExportAngle`|[`Data1DFileAndFormat`]({{< ref "data1dformat" >}})|--|File format and file name under which to save calculated A-B-C angle histogram.|
