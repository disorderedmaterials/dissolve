---
title: Coordinates
description: Coordinate import/export
---

## Import

For all of the following file formats only atomic coordinate information is generally used - i.e. species or connectivity information is not derived from the input.

### File Types

|Keyword|Description|
|:---:|-----------|
|`dlpoly`|CONFIG or REVCON file written by [DL_POLY](https://www.scd.stfc.ac.uk/Pages/DL_POLY.aspx).|
|`epsr`|Ato file written by [EPSR](https://www.isis.stfc.ac.uk/Pages/Empirical-Potential-Structure-Refinement.aspx).|
|`moscito`|Structure file (*.str) written by the [Moscito code](http://139.30.122.11/MOSCITO/).|
|`xyz`|XMol-style xyz coordinates. Line 1 contains the number of atoms N. Line 2 contains a title string. The next N lines contain "element  rx  ry  rz". |

### Options

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|

## Export

### File Types

|Keyword|Description|
|:---:|-----------|
|`dlpoly`|CONFIG file suitable for [DL_POLY](https://www.scd.stfc.ac.uk/Pages/DL_POLY.aspx).|
|`xyz`|XMol-style xyz coordinates.|
