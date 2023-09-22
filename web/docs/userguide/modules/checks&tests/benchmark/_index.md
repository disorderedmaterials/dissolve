---
title: Benchmark (Module)
linkTitle: Benchmark
description: Perform benchmarking on a variety of functions
---

## Overview

The `Benchmark` module records timing information through running selected functions on a given [configuration]({{< ref "configurations" >}}).

## Options

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Configuration`|`Configuration`|--|{{< required-label >}}Specifies the configuration on which to operate.|

### Control

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`N`|`int`|`5`|Number of times to run each benchmark in order to form average|
|`Save`|`bool`|`true`|Whether to save new timings to the restart file|

### Tests

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`TestIntraEnergy`|`bool`|`true`|Whether to benchmark intramolecular energy calculation|
|`TestInterEnergy`|`bool`|`true`|Whether to benchmark interatomic energy calculation|
|`TestGenerator`|`bool`|`true`|Whether to benchmark configuration generator|
|`TestRDFCells`|`bool`|`true`|Whether to benchmark the RDF cells method (to half-cell range)|
|`TestRDFSimple`|`bool`|`false`|Whether to benchmark the RDF simple method (to half-cell range)|
|`TestDistributors`|`bool`|`true`|Whether to benchmark molecule distributors|
