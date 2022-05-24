---
title: CalculateCN (Module)
linkTitle: CalculateCN
description: Calculate coordination numbers from an existing RDF
---

## Overview

The `CalculateCN` module calculates up to three coordination numbers from the output of a {{< gui-module "CalculateRDF" >}} module. Calculation of the first coordination number is always enabled, with the remaining two being optional.

## Keywords

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`SourceRDF`|`{{< gui-module "CalculateRDF" >}}`|`--`|{{< required-label >}}Specifies the {{< gui-module "CalculateRDF" >}} module from which the RDF is retrieved and the coordination numbers calculated.|

### Ranges

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`RangeA`|`double`<br/>`double`|`0.0`<br/>`3.0`|Specifies the distance range over which to calculate the first coordination number.|
|`RangeB`|`double`<br/>`double`|`3.0`<br/>`6.0`|Specifies the distance range over which to calculate the second coordination number.|
|`RangeBEnabled`|`bool`|`false`|Whether calculation of the second coordination number is enabled.|
|`RangeC`|`double`<br/>`double`|`6.0`<br/>`9.0`|Specifies the distance range over which to calculate the third coordination number.|
|`RangeCEnabled`|`bool`|`false`|Whether calculation of the third coordination number is enabled.|

### Test

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`TestRangeA`|`double`|`--`|For code testing - specifies the expected value of the first coordination number.|
|`TestRangeB`|`double`|`--`|For code testing - specifies the expected value of the second coordination number.|
|`TestRangeC`|`double`|`--`|For code testing - specifies the expected value of the third coordination number.|
|`TestThreshold`|`double`|`0.1`|For code testing - specifies the threshold difference value at which tests will fail.|
