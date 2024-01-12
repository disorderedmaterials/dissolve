---
title: ImportCoordinates (Node)
linkTitle: ImportCoordinates
description: Import coordinates into a configuration
---

{{< htable >}}
| | |
|--------|----------|
|Context|Generation|
|Name Required?|No|
|Branches|--|
{{< /htable >}}

## Overview

The `ImportCoordinates` node reads in coordinates from disk into the current configuration. The configuration must be set up in the normal way (i.e. with a box and the addition of species) since only the coordinates from the file are read and applied in order to the atoms in the configuration.

## Options

### Target

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Format`|[`CoordinatesFileAndFormat`]({{< ref "coordinatesformat" >}})|--|Format and filename of the target coordinates to read from disk.|

