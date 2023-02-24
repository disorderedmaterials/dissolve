---
title: OperateMultiply (Node)
linkTitle: OperateMultiply
description: Perform multiplication of numerical data
---

{{< htable >}}
| | |
|-|-|
|Context|Operate|
|Name Required?|No|
|Branches|--|
{{< /htable >}}

## Overview

The `OperateMultiply` node performs a multiplication operation on data supplied to it.

## Description

In the "Operate" context all `Operate*` nodes are passed data of some dimensionality (e.g. from one of the `Process*` nodes) which is then subjected to some kind of mathematical operation.

The `OperateMultiply` node simply multiplies the values (not the axes) of the supplied data by a specified constant. The target data may be of any dimensionality.

## Options

### Inputs

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Value`|`double`|`1.0`|Constant value to use as the multiplier.|
