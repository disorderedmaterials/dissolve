---
title: OperateSitePopulationNormalise (Node)
linkTitle: OperateSitePopulationNormalise
description: Normalise data to the populations of target site(s)
---

{{< htable >}}
| | |
|-|-|
|Context|Operate|
|Name Required?|No|
|Branches|--|
{{< /htable >}}

## Overview

The `OperateSitePopulationNormalise` node performs a division operation on data supplied to it, using the populations of target sites as sequential divisors.

## Description

In the "Operate" context all `Operate*` nodes are passed data of some dimensionality (e.g. from one of the `Process*` nodes) which is then subjected to some kind of mathematical operation.

The `OperateSitePopulationNormalise` node simply divides the values (not the axes) of the supplied data by the populations of one or more target sites. The target data may be of any dimensionality.

## Configuration

### Control Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Site`|`name`<br/>[`name...`]|--|{{< required-label >}} One or more `name`d [`SelectNode`s]({{< ref "selectnode" >}}) from which the site populations are used as divisors on the data. If more than one [`SelectNode`s]({{< ref "selectnode" >}}) is given, the divisions are made sequentially.|
