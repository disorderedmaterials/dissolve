---
title: AvgMol (Module)
linkTitle: AvgMol
description: Calculate the average atomic coordinates for a particular species
---


## Overview

The `AvgMol` module calculates the average geometry of a species using an oriented site on that species as a reference. The average geometry is accumulated over all molecules of that type in the configuration.

Results of the average molecule calculation can be used as a display reference in the {{< module "SDF" >}} module.

## Options

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Configuration`|`Configuration`|--|{{< required-label >}}Specifies the configuration on which to operate.|

### Sites

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Site`|`Site`|--|{{< required-label >}}Target site about which to calculate the average species geometry. Must be an oriented site.|

## Export

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`ExportCoordinates`|[`SpeciesExportFileFormat`]({{< ref speciesformat >}})|--|File and format to export average species coordinates to.|
