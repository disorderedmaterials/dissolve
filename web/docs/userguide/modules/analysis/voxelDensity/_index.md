---
title: VoxelDensity (Module)
linkTitle: VoxelDensity
description: Histogram of atomic property (mass, atomic number, scattering length density) over 3D, voxelised unit cell model. 
---

## Overview

The `VoxelDensity` module computes a voxelised (3D discretisation) map of the unit cell, in terms of a user-selected voxel volume, and a 'target property' - one of: atomic mass, atomic number, or scattering length density.

This 3D map is then converted to a histogram displaying the target property distribution over the unit cell voxels.

The algorithm can also be assigned to a restricted subset of chemical species, and the histogram range and bin width can be adjusted.

## Options

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Configuration`|`Configuration`|--|{{< required-label >}}Specifies the configuration on which to operate.|

### Species

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`RestrictToSpecies`|`Species ...`|--|Restrict voxel 3D map to only molecules of the specified species. Molecules of other species types remain at their current positions.|

### Ranges

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`BinRange`|`double`<br/>`double`<br/>`double`|`0.0`<br/>`10.0`<br/>`0.05`|Range (min, max, bin width) of histogram binning.|

### Analysis

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`VoxelSideLength`|`double`|The side length of an analysis voxel.|

## Export

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Export`|[`Data1DFileAndFormat`]({{< ref "data1dformat" >}})|--|File format and file name under which to save calculated 1D histogram data.|

