---
title: GeometryOptimisation (Module)
linkTitle: GeometryOptimisation
description: Run an energy minimisation in order to optimise geometry
---

## Overview

The `GeometryOptimisaion` module performs an energy-guided geometry optimisation on the target configuration.

## Options

### Targets
|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Configuration`|`Configuration`|--|{{< required-label >}}Target configuration on which to operate.|

### Control

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`MaxCycles`|`int`|`1000`|Maximum number of minimisation cycles to perform|
|`StepSize`|`double`|`1.0e-5`|Initial step size to employ|
|`Tolerance`|`double`|`1.0e-4`|Tolerance controlling convergence of algorithm|
