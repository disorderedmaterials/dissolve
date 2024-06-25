---
title: AddPair (Node)
linkTitle: AddPair
description: Insert correlated molecule pair into a box
---

{{< htable >}}
| | |
|--------|----------|
|Context|Generation|
|Name Required?|No|
|Branches|--|
{{< /htable >}}

## Overview

The `AddPair` node is a companion to the {{< node "Add" >}} node, but instead operates on a pair of species. The population refers to a population of pairs of species which are added at their current coordinates, then translated / rotated uniformly in order to maintain their relative coordinates.

Periodic species cannot be used by this node, nor can {{< node "CoordinateSets" >}}.

## Description

For full details on the possible options, see the {{< node "Add" >}} node.

## Options

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`SpeciesA`|`name`|--|{{< required-label >}} Target species A of the pair to add.|
|`SpeciesB`|`name`|--|{{< required-label >}} Target species B of the pair to add.|
|`Density`|[`expr`]({{< ref "expressions" >}})<br/>[`DensityUnit`]({{< ref "densityunit" >}})|`0.1 atoms/A3`|Density at which to add the target species. Note that the use of this value differs according to the selected `BoxAction` (see above).|
|`Population`|[`expr`]({{< ref "expressions" >}})|`0`|Population of the pairs to add.|

### Box Modification

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`BoxAction`|[`BoxActionStyle`]({{< ref "boxactionstyle" >}})|`AddVolume`|Action to take on the Box geometry / volume on addition of the species|
|`ScaleA`|`bool`|`true`|Whether to scale the A cell axis when changing the cell volume.|
|`ScaleB`|`bool`|`true`|Whether to scale the B cell axis when changing the cell volume.|
|`ScaleC`|`bool`|`true`|Whether to scale the C cell axis when changing the cell volume.|

### Positioning

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Positioning`|[`PositioningType`]({{< ref "positioningtype" >}})|`Random`|Positioning type for the pair.|
|`Region`|`name`|--|Region node controlling the location of inserted species into the configuration.|
|`Rotate`|`bool`|`true`|Whether to randomly rotate the molecule pair on insertion.|
