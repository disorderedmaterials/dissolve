---
title: Analyse (Module)
linkTitle: Analyse
description: Define and perform a custom analysis routine
---

## Overview

The `Analyse` module allows a custom analysis procedure to be defined, allowing almost any quantity to be calculated from a given configuration.

## Keywords

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Configuration`|`Configuration`|`--`|{{< required-label >}}Specifies the configuration on which to operate.|

## Control

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Analyser`|[`Procedure`]({{< ref "procedures" >}})|--|Specifies a complete [`Procedure`]({{< ref "procedures" >}}) encompassing the analysis routine to perform on the target configuration.|
