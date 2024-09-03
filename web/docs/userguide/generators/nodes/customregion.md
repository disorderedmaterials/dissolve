---
title: CustomRegion (Node)
linkTitle: CustomRegion
description: Constructs a custom region according to a specified equation
---

{{< htable >}}
| | |
|-|-|
|Context|Generation|
|Name Required?|Yes|
|Branches|--|
{{< /htable >}}

## Overview

The `CustomRegion` node generates a region defined by an arbitrary equation.

## Description

The `CustomRegion` node assesses each voxel of the current box against a provided [`Expression`]({{< ref expressions >}}) and some defined "acceptable" numerical limits on the result of that expression. The real and reciprocal space coordinates of each voxel centroid are set in local variables on the node, and which can be accessed in the [`Expression`]({{< ref expressions >}}) in order to enter them into the equation.

## Local Variables

The node sets the following local variables:

|Parameter|Description|
|:--------|-----------|
|`x`|The x coordinate of the current voxel centroid being assessed|
|`xFrac`|The fractional x coordinate of the current voxel centroid being assessed|
|`y`|The y coordinate of the current voxel centroid being assessed|
|`yFrac`|The fractional y coordinate of the current voxel centroid being assessed|
|`z`|The z coordinate of the current voxel centroid being assessed|
|`zFrac`|The fractional z coordinate of the current voxel centroid being assessed|

## Options

### Definition

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Expression`|[`Expression`]({{< ref expressions >}})|--|Expression to evaluate per-voxel|
|`Maximum`|`double`|`1.0`|Maximum value of the expression value to accept as representing a valid voxel|
|`Minimum`|`double`|`0.0`|Minimum value of the expression value to accept as representing a valid voxel|

### Grid

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`VoxelSize`|`double`|`1.0`|Cubic voxel length to use when partitioning space. A smaller voxel size gives a more fine-grained region, at the expense of memory and speed.|
|`Invert`|`bool`|`false`|Invert the logic used to determine free space in the region.|
