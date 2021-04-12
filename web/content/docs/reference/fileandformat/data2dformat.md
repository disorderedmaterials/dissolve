---
title: Data2D
description: Two-dimensional data import/export
---

## Known Formats

|Keyword|Import|Export|Description|
|:-----:|:----:|:----:|-----------|
|`block`||&check;|Single-column block data format, with values in slices along $y$ written sequentially, and slices separated with a blank line.|
|`cartesian`|&check;|&check;|Cartesian data format with "X Y f(X,Y)" provided one set per line. These points are mapped to the appropriate bin on $x$ and $y$ axes specified by the `XAxis` and `YAxis` options which *must* be supplied.|

## Options

### Import

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`XAxis`|`$x_{min}$`</br>`$x_{max}$`</br>`$x_{delta}$`|--|Min, max, and delta to assume for x axis|
|`YAxis`|`$y_{min}$`</br>`$y_{max}$`</br>`$y_{delta}$`|--|Min, max, and delta to assume for y axis|
