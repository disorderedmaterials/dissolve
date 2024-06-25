---
title: PickProximity (Node)
linkTitle: PickProximity
description: Pick species molecules near to other molecules
---

{{< htable >}}
| | |
|--------|----------|
|Context|Generation|
|Name Required?|Yes|
|Branches|--|
{{< /htable >}}

## Overview

The `PickProximity` node picks (selects) molecules of any species type if they are near to a given number of molecules of another type.

## Options

### Proximal Species

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Species`|`name`|--|Surrounding species to be counted.|
|`MinCount`|`int`|--|Minimum number of species required to permit selection.|
|`MaxCount`|`int`|--|Maximum number of species required to permit selection.|
|`MinDistance`|`double`|--|Minimum distance for picking (Angstroms).|
|`MaxDistance`|`double`|--|Maximum distance for picking (Angstroms).|

### Source

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Source`|`name`|--|The name of an existing picking node from which to pick molecules for the current selection. If not specified, any molecule in the current configuration may be picked.|
