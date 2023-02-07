---
title: Pick (Node)
linkTitle: Pick
description: Pick species molecules from anywhere inside a box
---

{{< htable >}}
| | |
|--------|----------|
|Context|Generation|
|Name Required?|Yes|
|Branches|--|
{{< /htable >}}

## Overview

The `Pick` node picks (selects) all molecules of a given species type from the current configuration, for further processing by other nodes.

## Options

### Target

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Species`|`name`|--|Add target species to pick - all molecules of the target species will be added to the node's internal selection list. More than one species name may be given to the keyword if it is desired to pick multiple species.|

### Source

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Source`|`name`|--|The name of an existing picking node from which to pick molecules for the current selection. If not specified, any molecule in the current configuration may be picked.|
