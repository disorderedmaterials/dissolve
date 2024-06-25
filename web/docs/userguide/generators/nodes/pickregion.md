---
title: PickRegion (Node)
linkTitle: PickRegion
description: Pick species molecules from a specific region
---

{{< htable >}}
| | |
|--------|----------|
|Context|Generation|
|Name Required?|Yes|
|Branches|--|
{{< /htable >}}

## Overview

The `PickRegion` node picks (selects) all molecules of any species type from a specified region, for further processing by other nodes.

## Options

### Control

### Target

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Region`|`name`|--|Region from which species molecules can be picked.|

### Source

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Source`|`name`|--|The name of an existing picking node from which to pick molecules for the current selection. If not specified, any molecule in the current configuration may be picked.|
