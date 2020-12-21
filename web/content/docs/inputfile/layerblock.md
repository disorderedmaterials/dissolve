---
title: Layer Block
description: Defines a sequence of modules
---

## Overview

The `Layer` block contains a number of [modules]({{< ref "modules" >}}) to run, in sequence, as part of the main simulation. Multiple `Layer` blocks can be defined in order to sensibly group together different parts of a particular simulation (e.g. equilibration, evolution, analysis, export etc.)

The block keyword itself takes a single (required) argument - the name of the layer. This name is used to reference the configuration when, for example, specifying [module]({{< ref "moduleblock" >}}) targets.

## Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Disabled`|--|--|Disables execution of the layer|
|`EndLayer`|--|--|Indicates the end of the current `Layer` block.|
|`Frequency`|`int`|`1`|Frequency at which this layer will run, relative to the main iteration counter. A value of `1` means the layer will run on every iteration, a value of `10` means the layer will only run every 10th iteration, etc.|
|`Module`|[`Module`]({{< ref "modules" >}})<br/>`[name]`|--|Begins a [`Module`]({{< ref "moduleblock" >}}) block setting up an instance of a module of the type specified, with the optional unique `name`. If a `name` is not provided, a unique one will be generated automatically.|

## Example

```r
Layer  'MyLayer'

  Frequency  5

  Module  'A'
    ...
  EndModule

  Module  'B'  'SpecialModule'
    ...
  EndModule

EndLayer
```
