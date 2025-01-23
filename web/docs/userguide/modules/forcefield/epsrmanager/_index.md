---
title: EPSR Manager (Module)
linkTitle: EPSR Manager
description: Modify and average the empirical potential.
---

## Overview

The `EPSR Manager` module allows for the modification, by a fixed multiplier, and averaging over a set number of iterations, of empirial potentials that is calculated via the {{< module "EPSR" >}} module. It can also be used to combine the potentials from several {{< module "EPSR" >}} modules.

## Options

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Target`|`EPSR`|--|{{< required-label >}}Specifies the EPSR modules on which to operate.|

### Potential Scaling

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`PotScale`|`Potential Set`|--|{{< required-label >}}Specifies the Potential Pair and the modifier to use e.g. Si-O=2.0 would increase the Si-O potential by a factor of 2.0.|

### Averaging

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Averaging`|`int`|`OFF`|Number of historical datasets to combine into final potentials|
|`AveragingScheme`|[`AveragingScheme`]({{< ref "averagingscheme" >}})|`Linear`|Weighting scheme to use when averaging data|
