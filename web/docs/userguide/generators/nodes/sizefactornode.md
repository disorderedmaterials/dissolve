---
title: SizeFactor (Node)
linkTitle: SizeFactor
description: Apply a size factor to a configuration
---

{{< htable >}}
| | |
|--------|----------|
|Context|Generation|
|Name Required?|No|
|Branches|--|
{{< /htable >}}

## Overview

The `SizeFactor` node applies a "size factor" scaling (to use the terminology of the original EPSR) to the current configuration, effectively scaling the box lengths and the centres of mass of molecules in order to move them apart. The intent of this operation is typically to prevent any rings within, for instance, aromatic species from being interlocked following the random initial insertion of molecules, but can be useful in the initial creation of many systems.

When a framework species is present in the box scaling its centre of mass position is obviously ineffective and so the scaling is applied to every atomic coordinate, "expanding" the framework. Care should be taken in such cases, and evolution methods altering intramolecular terms should not be applied until the box has reached its nominal size.

## Method

The value specified for the size factor represents the initial scaling to apply to the box. The simulation should proceed as normal with an equilibration stage, typically only involving pure atomic or molecular Monte Carlo moves. At the end of each iteration the total intermolecular (not interatomic) pair potential energy is checked and, if negative, the scaling factor is reduced slightly and the system "shrinks" back to its nominal, normal scaling value of 1.0.

Until the size factor reduces to 1.0 no fitting, processing, or analysis of the configuration should be performed. The main configuration shows a warning if the size factor is not yet 1.0, and most pre-defined module layers are set not to run if the size factor is anything but 1.0.

## Options

### Target

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`SizeFactor`|`double`|`10.0`|Initial size factor scaling to apply to the configuration.|
