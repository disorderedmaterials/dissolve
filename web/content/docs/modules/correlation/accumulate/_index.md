---
title: Accumulate (Module)
linkTitle: Accumulate
description: Accumulate (average) g(r) and S(Q) partial
---

## Overview

The `Accumulate` takes radial distribution function or structure factor data from a correlation module (such as [`RDF`]({{< ref "rdf" >}}) or [`NeutronSQ`]({{< ref "neutronsq" >}})) and accumulates an online average. As such it can be used to form a smoothed set of partials (and their total) for further analysis, saving to plot in production-quality graphs etc. 

The averaging performed by the `Accumulation` module differs from that performed in the correlation modules themselves - there, the average is a moving average, with `N` sets of partials stored in memory or in the restart file. Much like the various `Calculate` modules, the `Accumulation` module forms an online average with no history. 

A target module containing partial set data must be provided, along with the type of target data to accumulate (since in the case of the [`NeutronSQ`]({{< ref "neutronsq" >}}), for instance, both g(r) and S(Q) partial sets are generated).

## Configuration

### Test Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Data1D`|`TargetData`<br/>[`Data1DFileAndFormat`]({{< ref "data1d" >}})|--|Filename and format of external one-dimensional data to be read in and tested against the named internal `TargetData`. This keyword may be specified multiple times in order to test many datasets sequentially.|
|`Data2D`|`TargetData`<br/>[`Data2DFileAndFormat`]({{< ref "data2d" >}})|--|Filename and format of external two-dimensional data to be read in and tested against the named internal `TargetData`. This keyword may be specified multiple times in order to test many datasets sequentially.|
|`Data3D`|`TargetData`<br/>[`Data3DFileAndFormat`]({{< ref "data3d" >}})|--|Filename and format of external three-dimensional data to be read in and tested against the named internal `TargetData`. This keyword may be specified multiple times in order to test many datasets sequentially.|
|`ErrorType`|[`ErrorType`]({{< ref "errortype" >}})|`Euclidean`|Error metric to use when comparing the datasets within this module instance.|
|`InternalData1D`|`TargetDataA`<br/>`TargetDataB`|--|Target internal datasets `TargetDataA` amd `TargetDataB` to compare and calculate error. This keyword may be specified multiple times in order to test many datasets sequentially.|
|`Threshold`|`double`|`5.0e-3`|Threshold value for error, above which the test fails.|

