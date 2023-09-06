---
title: Bragg (Module)
linkTitle: Bragg
description: Calculate Bragg reflection data
---

## Overview

The `Bragg` module calculates the Bragg reflections over a specified Q-range, storing the intensities for use by other modules (e.g. {{< module "SQ" >}}).

## Description

### Basic Theory

{{< todo-label >}}

### Instrumental Broadening

The Bragg module does not handle broadening of the calculated reflections into "observed" peak structures - this is handled by the module referencing the reflection data.

## Options

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Configuration`|`Configuration`|--|{{< required-label >}}Target configuration on which to operate.|

### Control

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`QDelta`|`qdelta`|`0.01`|Resolution (bin width) in Q to use when detecting reflections.|
|`QMax`|`qmax`|`30.0`|$Q_{max}$ limit for reflection calculation. Any reflections at $Q$ values above this value will be ignored.|
|`QMin`|`qmin`|`0.01`|$Q_{min}$ limit for reflection calculation. Any reflections at $Q$ values below this value will be ignored.|
|`Multiplicity`|`h`<br/>`k`<br/>`l`|`1`<br/>`1`<br/>`1`|Multiplicities of the unit cell in the target configuration. If the target configuration represents a single unit cell, the default of `[1 1 1]` should be used. If it is a 2x2x2 supercell, for instance, then the multiplicity should be set to `[2 2 2]`. Failing to set the multiplicity correctly will result in incorrect intensities in the calculated reflections.|

### Averaging

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Averaging`|`int`|`5`|Number of historical datasets to combine into final reflections|
|`AveragingScheme`|[`AveragingScheme`]({{< ref "averagingscheme" >}})|`Linear`|Weighting scheme to use when averaging data|

## Export

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`SaveReflections`|`bool`|`false`|Whether to save Bragg reflection data to disk after calculation. A separate file containing Q values and raw intensities is written for each individual atomic partial between types $i$ and $j$.|

