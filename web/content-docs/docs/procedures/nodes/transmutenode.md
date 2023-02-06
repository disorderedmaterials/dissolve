---
title: Transmute (Node)
linkTitle: Transmute
description: Transmute molecules in a selection into another species
---

{{< htable >}}
| | |
|--------|----------|
|Context|Generation|
|Name Required?|No|
|Branches|--|
{{< /htable >}}

## Overview

The `Transmute` node transmutes all molecules in the supplied selection into molecules of the specified type.

## Options

### Target

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Species`|`Species ...`|--|One or more species that are to be transmuted into the target species.|
|`Selection`|`name`|--|The name of an existing picking node supplying target molecules for transmutation.|

### Target

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Target`|`Species`|--|Target species to transmute selected molecules in to.|
