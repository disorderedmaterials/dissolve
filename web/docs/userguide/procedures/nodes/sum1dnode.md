---
title: Sum1D (Node)
linkTitle: Sum1D
description: Sum 1D data over ranges
---

{{< htable >}}
| | |
|-|-|
|Context|Analysis|
|Name Required?|No|
|Branches|--|
{{< /htable >}}

## Overview

The `Sum1D` node is used to form summations over up to three defined regions of data produced by a {{< node "Process1D" >}} node.

## Description

Once a {{< node "Process1D" >}} node has processed data into something meaningful the `Sum1D` node can perform a summation of that data over defined ranges, providing an averaged value of the quantity.

Note that, of the three ranges, the first ("A") is always calculated.

## Options

### Source

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Instantaneous`|`bool`|`false`|Whether the processed data should reflect the accumulated average over the defined ranges (`false`) or the "instantaneous" value (`true`).|
|`SourceData`|`name`|--|{{< required-label >}} The `name` of a {{< node "Process1D" >}} node containing the target data.|

### Ranges

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`RangeA`|`min`</br>`max`|`0.0`</br>`3.0`|First range to target.|
|`RangeB`|`min`</br>`max`|`3.0`</br>`6.0`|Second range to target.|
|`RangeC`|`min`</br>`max`|`6.0`</br>`9.0`|Third range to target.|
|`RangeBEnabled`|`bool`|`false`|Whether the second range is enabled for calculation.|
|`RangeCEnabled`|`bool`|`false`|Whether the third range is enabled for calculation.|
