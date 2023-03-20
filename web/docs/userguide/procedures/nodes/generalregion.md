---
title: GeneralRegion (Node)
linkTitle: GeneralRegion
description: Constructs a general region
---

{{< htable >}}
| | |
|-|-|
|Context|Generation|
|Name Required?|Yes|
|Branches|--|
{{< /htable >}}

## Overview

The `GeneralRegion` node generates a region encapsulating the free or void space within a box - it is therefore particularly suitable as a region for the insertion of new molecules by an {{< gui-node "Add" >}} node.

## Description

The `GeneralRegion` node assesses the current contents of a configuration and locates "free volume" into which molecules can be added. Volume is excluded based on the proximity of existing atoms in the configuration - a volume element is excluded if any atom is within the tolerance distance specified of its coordinate centre.

## Configuration

### Control

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Tolerance`|`distance`|`2.0`|Minimum distance at which an existing atom may reside next to a voxel's central coordinate. If an atom exists at a shorter distance, that voxel is excluded.|
|`VoxelSize`|`l`|`2.0`|Cubic voxel length to use when partitioning space. A smaller voxel size gives a more fine-grained region, at the expense of memory and speed.|
