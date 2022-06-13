---
title: CoordinateSets (Node)
linkTitle: CoordinateSets
description: Generate a population of molecules from a Species
---

{{< htable >}}
| | |
|--------|----------|
|Context|Generation|
|Name Required?|Yes|
|Branches|--|
{{< /htable >}}

## Overview

The `CoordinateSets` node generates a population of molecules from a reference species.

## Description

The `CoordinateSets` node generates a population of sets of coordinates of a specified species, using a particular evolution algorithm.  These sets can then be used by another node, for instance the {{< gui-node "Add" >}} node in order to add "unique" molecules of different conformations to a box, rather than identical copies of the same species.

The coordinate set population is only generated once (the first time the node is run in the procedure) - subsequently the existing population will be used unless the `Force` option is enabled.

## Configuration

### Control

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`DeltaT`|`double`|`5.0e-4`|Timestep to use in "MD" generation method.|
|`File`|[TrajectoryFileAndFormat]({{< ref "trajectoryformat" >}})|--|File and format specifying coordinates to read in if `Source` == "File".|
|`Force`|`bool`|`false`|Whether the generation of coordinate sets should proceed every time the node is executed. Otherwise, the population is generated on the first call only.|
|`NSets`|`int`|`100`|Number of sets to generate.|
|`NSteps`|`int`|`2000`|Number of steps between capture of sets in MD generation method.|
|`Source`|[`CoordinateSetSource`]({{< ref "coordinatesetsource" >}})|`MD`|Source for coordinate sets.|
|`Species`|`name`|--|{{< required-label >}} Target species to for which to generate sets of coordinates.|
