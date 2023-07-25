---
title: CylindricalRegion (Node)
linkTitle: CylindricalRegion
description: Constructs a cylindrical region
---

{{< htable >}}
| | |
|-|-|
|Context|Generation|
|Name Required?|Yes|
|Branches|--|
{{< /htable >}}

## Overview

The `GeneralRegion` node generates a region encapsulating the free or void space within a box - it is therefore particularly suitable as a region for the insertion of new molecules by an {{< node "Add" >}} node.

## Description

The `GeneralRegion` node assesses the current contents of a configuration and locates "free volume" into which molecules can be added. Volume is excluded based on the proximity of existing atoms in the configuration - a volume element is excluded if any atom is within the tolerance distance specified of its coordinate centre.

## Options

### Definition

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`OriginFrac`|`Vec3<double>`|`0.0 0.0 0.0`|Origin of vector in fractional coordinates|
|`Radius`|`double`|`2.0`|Cylinder radius|
|`Vector`|`Vec3<double>`|`0.0 0.0 1.0`|Vector of cylinder|

### Grid

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`VoxelSize`|`double`|`1.0`|Cubic voxel length to use when partitioning space. A smaller voxel size gives a more fine-grained region, at the expense of memory and speed.|
|`Invert`|`bool`|`false`|Invert the logic used to determine free space in the region.|
