---
title: PickCylinder (Node)
linkTitle: Pick
description: Pick species molecules from a box
---

{{< htable >}}
| | |
|--------|----------|
|Context|Generation|
|Name Required?|Yes|
|Branches|--|
{{< /htable >}}

## Overview

The `Pick` node picks (selects) all molecules of a given species type from the current configuration, for further processing by other nodes.

## Description

Any molecule is picked if the distance between it's centre of geometry and the closest point on the line defining the cylinder position is less than or equal to the radius of the cylinder.

The two points defining the line on which the cylinder exists are given by:

$l_0 = (o_x, o_y, o_z) $

$l_1 = (o_x + x, o_y + y, o_z + z) $

...where $(o_x, o_y, o_z)$ are the coordinates of the origin in real-space, determined from the fractional cell coordinates $(f_x, f_y, f_z)$ provided with the `OriginFrac` keyword.

The shortest distance $d$ from the point $p$ (the molecule center of geometry) to the line is then:

$d = \frac{\lvert(p - l_0) \times (p - l_1)\rvert}{\lvert l_1 - l_0 \rvert} $

For the full derivation of this equation see https://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html.

## Configuration

### Control Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`OriginFrac`|`fx`</br>`fy`</br>`fz`| `0.0`</br>`0.0`</br>`0.0`|Fractional cell coordinates representing one point on the line describing the cylinder.|
|`Radius`|`double`|5.0|Radius, in Angstroms, of the cylinder.|
|`Source`|`name`|--|The name of an existing picking node from which to pick molecules for the current selection. If not specified, any molecule in the current configuration may be picked.|
|`Vector`|`x`</br>`y`</br>`z`| `0.0`</br>`0.0`</br>`1.0`|Defines the direction of orientation of the cylinder from the origin point.|
