---
title: Integrate1D (Node)
linkTitle: Integrate1D
description: Perform integration on 1D data
---

{{< htable >}}
| | |
|-|-|
|Context|Operate|
|Name Required?|No|
|Branches|--|
{{< /htable >}}

## Overview

The `Integrate1D` node integrates up to three regions of a supplied {{< gui-node "Process1D" >}} node.

## Options

### Target

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`SourceData`|{{< gui-node "Process1D" >}}|--|Process1D node containing the data to integrate.|

### Ranges

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`RangeA`|`double double`|--|X range for first integration region.|
|`RangeB`|`double double`|--|X range for second integration region.|
|`RangeC`|`double double`|--|X range for third integration region.|
