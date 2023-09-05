---
title: RegionalGlobalPotential (Node)
linkTitle: RegionalGlobalPotential
description: Create a discretized global potential affecting all atoms
---

{{< htable >}}
| | |
|--------|----------|
|Context|Generation|
|Name Required?|No|
|Branches|--|
{{< /htable >}}

## Overview

The `RegionalGlobalPotential` node allows an additional, global potential to be defined in a configuration, acting on all atoms. The potential is discretised over a voxel grid, with the value of the potential at the centre of each voxel determined by evaluating a user-defined [expression]({{< ref expressions >}}). Limits on the "allowable" values of the function can be set so as to more forcibly restrict molecules into specific regions, with a penalty function in the form of a power law applied to values outside of the threshold range. The evaluated value may also be offset for the purposes of this penalty evaluation, in order to better handle functions varying between, for example, 0.0 and 1.0, and which are not typically compatible with a power law penalty.

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
|`Offset`|`double`|`0.0`|Offset to apply to calculated value when assessing threshold and penalty|
|`PenaltyPower`|`double`|`1.0`|Power to apply to the (offset) value if out of threshold range|

### Grid

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`VoxelSize`|`double`|`1.0`|Cubic voxel length to use when partitioning space. A smaller voxel size gives a more fine-grained region, at the expense of memory and speed.|
