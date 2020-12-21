---
title: Forces
description: Force import/export
---

## Import

### File Types

|Keyword|Description|
|:---:|-----------|
|`dlpoly`|Forces from a CONFIG or REVCON file written by [DL_POLY](https://www.scd.stfc.ac.uk/Pages/DL_POLY.aspx).|
|`moscito`|Forces from structure file (*.str) written by the [Moscito code](http://139.30.122.11/MOSCITO/).|
|`simple`|Simple three-column file. Line 1 contains the number of atoms N, with the next N lines containing "fx  fy  fz" for a single atom. |

### Options

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Factor`|`double`|`1.0`|Factor by which to multiply force values once they have been read in, enabling units to be converted to the internal units used by Dissolve (10 J mol).|

## Export

### File Types

|Keyword|Description|
|:---:|-----------|
|`simple`|Simple three-column file. Line 1 contains the number of atoms N, with the next N lines containing "fx  fy  fz" for a single atom. |
