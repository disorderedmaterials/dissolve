---
title: Data3D
description: Three-dimensional data import/export
---

## Known Formats

|Keyword|Import|Export|Description|
|:-----:|:----:|:----:|-----------|
|`block`||&check;|Single-column block data format, with values in slices along $y$ written sequentially, and slices separated with a blank line.|
|`cartesian`|&check;|&check;|Cartesian data format with "X Y Z f(X,Y,Z)" provided one set per line. These points are mapped to the appropriate bin on the $x$, $y$, and $z$ axes specified by the `XAxis`, `YAxis`, and `ZAxis` options which *must* be supplied.|
|`pdens`||&check;|File following the format of output from the `pdens` program of [dlputils](https://www.projectaten.com/dlputils).|

## Options

### Import

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`XAxis`|`$x_{min}$`</br>`$x_{max}$`</br>`$x_{delta}$`|--|Min, max, and delta to assume for x axis|
|`YAxis`|`$y_{min}$`</br>`$y_{max}$`</br>`$y_{delta}$`|--|Min, max, and delta to assume for y axis|
|`ZAxis`|`$z_{min}$`</br>`$z_{max}$`</br>`$z_{delta}$`|--|Min, max, and delta to assume for z axis|
