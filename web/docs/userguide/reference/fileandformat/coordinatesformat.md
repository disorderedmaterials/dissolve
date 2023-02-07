---
title: Coordinates
description: Coordinate import/export
---

## Known Formats

|Keyword|Import|Export|Description|
|:-----:|:----:|:----:|-----------|
|`dlpoly`|&check;|&check;|CONFIG or REVCON file written by [DL_POLY](https://www.scd.stfc.ac.uk/Pages/DL_POLY.aspx).|
|`epsr`|&check;||Ato file written by [EPSR](https://www.isis.stfc.ac.uk/Pages/Empirical-Potential-Structure-Refinement.aspx).|
|`moscito`|&check;||Structure file (*.str) written by the [Moscito code](http://139.30.122.11/MOSCITO/).|
|`xyz`|&check;|&check;|XMol-style xyz coordinates. Line 1 contains the number of atoms N. Line 2 contains a title string. The next N lines contain "element  rx  ry  rz". |

## Notes

### Import

For all file formats only atomic coordinate information is generally used - i.e. species or connectivity information is not derived from the input.
