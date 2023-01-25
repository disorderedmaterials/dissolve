---
title: Box (Node)
linkTitle: Box
description: Define / create a unit cell
---

{{< htable >}}
| | |
|-|-|
|Context|Generation|
|Name Required?|No|
|Branches|--|
{{< /htable >}}

## Overview

The `Box` node creates a unit cell for the current configuration.

## Description

The sole purpose of the `Box` node is to set the geometry of the unit cell (box) for the current configuration.

## Options

### Definition

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Angles`|`alpha`<br/>`beta`<br/>`gamma`|`90.0`<br/>`90.0`<br/>`90.0`|Box angles $\alpha$, $\beta$, and $\gamma$.|
|`Lengths`|`lx`<br/>`ly`<br/>`lz`|`1.0`<br/>`1.0`<br/>`1.0`|Box lengths. These may be absolute or relative lengths.|
|`NonPeriodic`|`bool`|`false`|Whether the box is non-periodic.|
