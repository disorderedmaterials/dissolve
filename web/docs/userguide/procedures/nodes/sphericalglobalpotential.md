---
title: SphericalGlobalPotential (Node)
linkTitle: SphericalGlobalPotential
description: Create a spherical, global potential affecting all atoms
---

{{< htable >}}
| | |
|--------|----------|
|Context|Generation|
|Name Required?|No|
|Branches|--|
{{< /htable >}}

## Overview

The `SphericalGlobalPotential` node allows an additional, global potential to be defined in a configuration, acting on all atoms.

## Options

### Definition

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Potential`|[`Function1D`]({{< ref function1d >}})|--|Functional form and associated parameters for the potential.|
|`Origin`|`Vec3<double>`|`0.0 0.0 0.0`|Coordinate origin of the potential.|
|`Fractional`|`bool`|`false`|Whether the coordinate origin is specified in fractional cell coordinates.|
