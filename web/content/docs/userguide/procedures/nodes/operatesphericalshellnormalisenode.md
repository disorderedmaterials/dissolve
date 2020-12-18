---
title: OperateSphericalShellNormalise (Node)
linkTitle: OperateSphericalShellNormalise
description: Normalise 1D data to spherical shell volumes
---

{{< htable >}}
| | |
|-|-|
|Context|Operate|
|Name Required?|No|
|Branches|--|
{{< /htable >}}

## Overview

The `OperateSphericalShellNormalise` node is a specialised node which normalises values against spherical shell volumes. As such it is primarily used when normalising histograms in order to achieve a radial distribution function.

## Description

In the "Operate" context all `Operate*` nodes are passed data of some dimensionality (e.g. from one of the `Process*` nodes) which is then subjected to some kind of mathematical operation.

The `OperateSphericalShellNormalise` node can only operate on one-dimensional data (e.g. from a [`Process1D`]({{< ref "process1dnode" >}})) and assumes that the x axis of the data represents distance, as in a radial distribution function. From the x axis values, the volumes of spherical shells centred at those distances are calculated and used as divisors on the values.

## Configuration

### Control Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
