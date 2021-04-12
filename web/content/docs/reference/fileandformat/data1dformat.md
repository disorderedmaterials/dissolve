---
title: Data1D
description: One-dimensional data import/export
---

## Known Formats

|Keyword|Import|Export|Description|
|:-----:|:----:|:----:|-----------|
|`xy`|&check;|&check;|Two-column data format, with $x$ values representing discrete points or bin centres|
|`histogram`|&check;||Two-column data format, with $x$ values representing bin left-boundaries. The $x$ axis is converted internally to centre-bin values once reading is complete.|
|`mint01`|&check;||Gudrun "mint01" file format, with $Q$ values representing bin left-boundaries. The $Q$ ($x$) axis is converted internally to centre-bin values once reading is complete.|

## Options

### Import

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Error`|`int`|--|Column index in the file from which to read error values|
|`RemoveAverage`|`double`|--|Starting $x$ value from which to form an average $y$ value to subtract from the data|
|`Trim`|`$x_{min}$`</br>`$x_{max}$`|--|Discard any data points outside of the range specified|
|`X`|`int`|`1`|Column index in the file from which to read $x$ values|
|`Y`|`int`|`2`|Column index in the file from which to read $y$ values|

Note that if multiple keywords that modify the data are supplied, the order of operation is:

1. Remove points from the beginning of the data (controlled by `RemovePoints`)
2. Trim $x$ range of data (controlled by `Trim`)
3. Form and subtract average level (controlled by `RemoveAverage`)
