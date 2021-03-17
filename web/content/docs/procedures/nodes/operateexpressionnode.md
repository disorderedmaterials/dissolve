---
title: OperateExpression (Node)
linkTitle: OperateExpression
description: Perform division on numerical data
---

{{< htable >}}
| | |
|-|-|
|Context|Operate|
|Name Required?|No|
|Branches|--|
{{< /htable >}}

## Overview

The `OperateExpression` node performs a multiplication operation on data supplied to it, using the result of a numerical expression as the multiplier on a per-bin basis.

## Description

In the "Operate" context all `Operate*` nodes are passed data of some dimensionality (e.g. from one of the `Process*` nodes) which is then subjected to some kind of mathematical operation.

The `OperateExpression` node multiplies the values (not the axes) of the supplied data by the result of the supplied expression. The expression has access to the x, y, and z values of the current bin when performing the multiplication, and so the multiplier can vary according to zero or more of those quantities. The target data may be of any dimensionality.

## Configuration

### Control Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Value`|[`expr`]({{< ref "expressions" >}})|`1.0`|Value expression to use as the multiplier.|
