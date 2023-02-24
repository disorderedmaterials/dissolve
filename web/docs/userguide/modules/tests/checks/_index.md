---
title: Checks (Module)
linkTitle: Checks
description: Perform basic testing of geometry.
---

## Overview

The `Checks` module performs basic numerical checks on geometric quantities, and is largely used for testing purposes and is not necessarily useful for the casual user.

## Options

### Targets
|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Configuration`|`Configuration`|--|{{< required-label >}}Target configuration on which to operate.|

### Distance

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Distance`|`[int int double] ...`|--|Define reference distances between specific atoms to check|
|`DistanceThreshold`|`double`|`1.0e-3`|Threshold at which distance checks will fail (Angstroms)|

### Angle

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Angle`|`[int int int double] ...`|--|Define an angle between Atoms to be checked|
|`AngleThreshold`|`double`|`0.05`|Threshold at which angle checks will fail (Angstroms)|
