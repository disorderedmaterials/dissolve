---
title: CylindricalGlobalPotential (Node)
linkTitle: CylindricalGlobalPotential
description: Create a cylindrical global potential affecting all atoms
---

{{< htable >}}
| | |
|--------|----------|
|Context|Generation|
|Name Required?|No|
|Branches|--|
{{< /htable >}}

## Overview

The `CylindricalGlobalPotential` node allows an additional, global potential with cylindrical shape to be defined in a configuration, acting on all atoms and in a specified direction.

## Options

### Definition

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Potential`|[`Function1D`]({{< ref function1d >}})|--|Functional form and associated parameters for the potential.|
|`Origin`|`Vec3<double>`|`0.0 0.0 0.0`|Coordinate origin of the potential.|
|`Fractional`|`bool`|`false`|Whether the coordinate origin is specified in fractional cell coordinates.|
|`Vector`|`Vec3<double>`|`0.0 0.0 1.0`|Cylindrical vector along which the potential acts.|
