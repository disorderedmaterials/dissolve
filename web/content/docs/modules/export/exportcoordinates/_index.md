---
title: ExportCoordinates (Module)
linkTitle: ExportCoordinates
description: Export configuration coordinates
---

## Overview

The `ExportCoordinates` module writes the coordinate data from a target configuration to a named file, overwriting any existing file (unless the filename is suffixed with the current iteration number with the `TagWithIteration` option).

## Configuration

### Control Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Format`|[`CoordinatesFileAndFormat`]({{< ref "coordinatesformat" >}})|--|Format and filename of the coordinates file to write to disk.|
|`TagWithIteration`|`bool`|`false`|Whether to append the current iteration number to the filename. This allows individual configuration files to be written, rather than overwriting the same file.|
