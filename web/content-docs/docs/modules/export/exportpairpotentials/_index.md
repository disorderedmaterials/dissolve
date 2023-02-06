---
title: ExportPairPotentials (Module)
linkTitle: ExportPairPotentials
description: Export pair potential data
---

## Overview

The `ExportPairPotentials` module writes interatomic pair potential data to disk, one file per unique pair potential.

## Options

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Configuration`|`Configuration`|--|{{< required-label >}}Target configuration on which to operate.|

### File

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Format`|[`PairPotentialFileAndFormat`]({{< ref "pairpotentialformat" >}})|--|Format and base filename of the pair potential files to write to disk.|
