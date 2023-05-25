---
title: ExportConfiguration (Module)
linkTitle: ExportConfiguration
description: Export configuration coordinates
---

## Overview

The `ExportConfiguration` module writes the target configuration to a named file, overwriting any existing file (unless the filename is suffixed with the current iteration number with the `TagWithIteration` option).

## Options

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Configuration`|`Configuration`|--|{{< required-label >}}Target configuration on which to operate.|

### File

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Format`|[`ConfigurationFileAndFormat`]({{< ref "configurationformat" >}})|--|Format and filename of the coordinates file to write to disk.|
|`TagWithIteration`|`bool`|`false`|Whether to append the current iteration number to the filename. This allows individual configuration files to be written, rather than overwriting the same file.|
