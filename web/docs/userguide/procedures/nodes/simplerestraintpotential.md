---
title: SimpleRestraintPotential (Node)
linkTitle: SimpleRestraintPotential
description: Apply simple restraint potentials to individual atoms
---

{{< htable >}}
| | |
|--------|----------|
|Context|Generation|
|Name Required?|No|
|Branches|--|
{{< /htable >}}

## Overview

The `SimpleRestraintPotential` node allows atomic restraint potentials to be applied to one or more types of species in the configuration, and/or those in a [previously-picked selection]({{< ref "picknode" >}}). Each atom of every target molecule has assigned to it a targeted potential of the specified form/parameters with origin at its current coordinates.

## Options

### Definition

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Potential`|[`SimplePotential`]({{< ref simplepotential >}})|--|Functional form and associated parameters for the restraint potential.|

### Target

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Selection`|`name`|--|Restrain the atoms of all molecules selected by the specified picking node.|
|`Species`|`name`|--|Restrain the atoms of all molecules of the given species type(s).|
