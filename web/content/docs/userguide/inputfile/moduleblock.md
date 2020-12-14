---
title: Module Block
description: Creates and sets up a Module
---

## Overview

A `Module` block creates and completely defines a module, it's inputs and outputs, and all control parameters. Module blocks are defined within [layer blocks]({{< ref "layerblock" >}}).

The block keyword itself takes a single (required) argument (the [type of module]({{< ref "modules" >}}) to run) and an optional second argument specifying a unique name for the module, used to reference it from other modules in this or different layers.

## Basic Keywords

Every module recognises the following keywords, giving control over its basic operation and definition.

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Disabled`|--|--|Completely disables the module, preventing it from running within the current layer.|
|`EndModule`|--|--|Indicates the end of the current `Module` block.|
|`Frequency`|`n`|`1`|Frequency `n` at which the module should be run. A frequency of `1` indicates that the module will run every time the encompassing processing layer is run, a value of `5` runs the module on every 5th execution of the layer. Note that the [layer itself]({{< ref "layerblock" >}}) also has an associated frequency, relative the main loop counter.|

## Module Keywords

In addition to the basic keywords, each module has its own unique set of control keywords used to set up and define its behaviour. Refer to the individual [module pages]({{< ref "modules" >}}) for more information.

## Example

```r
Module  'TestModule'  'MyName'

  Frequency  5
  Disabled

  ModuleOption1  ...
  ModuleOption2  ...

  ...

EndModule
```
