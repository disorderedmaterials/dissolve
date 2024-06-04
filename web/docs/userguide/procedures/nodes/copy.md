---
title: Copy (Node)
linkTitle: Copy
description: Copy a configuration
---

{{< htable >}}
| | |
|--------|----------|
|Context|Generation|
|Name Required?|No|
|Branches|--|
{{< /htable >}}

## Overview

The `Copy` node copies the box and all molecules from a source configuration into the current one, providing e.g. a basis for further editing.

## Options

### Target

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Source`|`Configuration`|--|Source configuration to copy.|
|`Exclude`|`Species ...`|--|Species to exclude from the copy.|
|`CopyGlobalPotentials`|`bool`|`true`|Whether to copy any defined global potentials from the source configuration.|
