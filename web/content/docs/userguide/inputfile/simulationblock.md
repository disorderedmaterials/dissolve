---
title: Simulation Block
description: Control some general aspects of Dissolve's behaviour
---

## Overview

The `Simulation` block controls general operation of the code. It is largely deprecated, and will likely be removed in a future version.

## Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Seed`|`int`|--|Set the random seed to use in the simulation. If not specified, a new random seed is generated before the simulation begins.|

## Example

```r
Simulation
  Seed  81947653
EndSimulation
```
