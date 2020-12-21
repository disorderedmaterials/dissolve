---
title: OperateNumberDensityNormalise (Node)
linkTitle: OperateNumberDensityNormalise
description: Normalise data to the number density of target site(s)
---

{{< htable >}}
| | |
|-|-|
|Context|Operate|
|Name Required?|No|
|Branches|--|
{{< /htable >}}

## Overview

The `OperateNumberDensityNormalise` node performs a division operation on data supplied to it, using the number densities of target sites as sequential divisors.

## Description

In the "Operate" context all `Operate*` nodes are passed data of some dimensionality (e.g. from one of the `Process*` nodes) which is then subjected to some kind of mathematical operation.

The `OperateNumberDensityNormalise` node simply divides the values (not the axes) of the supplied data by the number density of one or more target sites. The target data may be of any dimensionality.

## Configuration

### Control Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Site`|`name`<br/>[`name...`]|--|{{< required-label >}} One or more `name`d [`SelectNode`s]({{< ref "selectnode" >}}) from which the site populations, along with the configuration box volume, are used to calculate number densities in atoms/A<sup>3</sup>, and which are then used as divisors on the data. If more than one [`SelectNode`s]({{< ref "selectnode" >}}) is given, the divisions are made sequentially.|
