---
title: DataTest (Module)
linkTitle: DataTest
description: Compare calculated data from modules with external or internal references
---

## Overview

`DataTest` is predominantly used in Dissolve's system tests for checking calculated one-, two-, or three-dimensional properties against external reference data. It also allows comparison between datasets within Dissolve, for example when ensuring the correctness and consistency of two different methods calculating the same thing.

## Configuration

### Test Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Data1D`|`TargetData`<br/>[`Data1DFileAndFormat`]({{< ref "data1dformat" >}})|--|Filename and format of external one-dimensional data to be read in and tested against the named internal `TargetData`. This keyword may be specified multiple times in order to test many datasets sequentially.|
|`Data2D`|`TargetData`<br/>[`Data2DFileAndFormat`]({{< ref "data2dformat" >}})|--|Filename and format of external two-dimensional data to be read in and tested against the named internal `TargetData`. This keyword may be specified multiple times in order to test many datasets sequentially.|
|`Data3D`|`TargetData`<br/>[`Data3DFileAndFormat`]({{< ref "data3dformat" >}})|--|Filename and format of external three-dimensional data to be read in and tested against the named internal `TargetData`. This keyword may be specified multiple times in order to test many datasets sequentially.|
|`ErrorType`|[`ErrorType`]({{< ref "errortype" >}})|`Euclidean`|Error metric to use when comparing the datasets within this module instance.|
|`InternalData1D`|`TargetDataA`<br/>`TargetDataB`|--|Target internal datasets `TargetDataA` and `TargetDataB` to compare and calculate error. This keyword may be specified multiple times in order to test many datasets sequentially.|
|`SampledDouble`|`TargetData`<br/>`double`|--|Target internal `SampledDouble` data `TargetData` and the real value to compare it against. This keyword may be specified multiple times in order to test many datasets sequentially.|
|`SampledVector`|`TargetData`<br/>[`ValueFileAndFormat`]({{< ref "valueformat" >}})|--|Filename and format of external value data to be read in and tested against the named internal `TargetData`, which is a `SampledVector`. This keyword may be specified multiple times in order to test many datasets sequentially.|
|`Threshold`|`double`|`5.0e-3`|Threshold value for error, above which the test fails.|
