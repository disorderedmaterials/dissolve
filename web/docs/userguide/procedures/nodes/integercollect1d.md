---
title: IntegerCollect1D (Node)
linkTitle: IntegerCollect1D
description: Accumulate a 1D histogram of explicit integer values
---

{{< htable >}}
| | |
|-|-|
|Context|Analysis|
|Name Required?|Yes|
|Branches|`SubCollect`|
{{< /htable >}}

## Overview

The `IntegerCollect1D` node is used to accumulate a histogram of a one-dimensional quantity, extracted from a `Calculate`-style nodes, with strictly integer binning. Unlike its `Collect1D` counterpart it is not necessary to specify the limits for the collection - in such a case all integer values will be binned.

## Description

Where the `Calculate*` nodes are responsible for calculating quantities, the `Collect*` nodes are responsible for accumulating averages, typically in histogram format.

`IntegerCollect1D` accumulates a histogram of a target value taken from a `Calculate*` node and binning into integer-sized bins. No post-processing (e.g. normalisation) is performed by `IntegerCollect1D` - see the {{< node "Process1D" >}}for processing options.

## Branching

The `IntegerCollect1D` node has a branch (accessed through the hidden `SubCollect` keyword) which is executed if the value is successfully binned (i.e. is within all ranges specified).

## Options

### Quantities / Ranges

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Maximum `|`int`|--|Defines the optional maximum limit for binning|
|`Minimum `|`int`|--|Defines the optional minimum limit for binning|
|`QuantityX`|`name`<br/>`[index]`|--|{{< required-label >}} The calculated observable to collect. The named node must be one of the `Calculate*` nodes. The `index` specifies which value to take from multi-dimensional observables, starting from zero for the first. If not specified, index` defaults to zero.|
