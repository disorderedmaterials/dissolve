---
title: Select (Node)
linkTitle: Select
description: Select a list of pre-defined sites on molecules
---

{{< htable >}}
| | |
|-|-|
|Context|Analysis|
|Name Required?|Yes|
|Branches|`ForEach`|
{{< /htable >}}

## Overview

The `Select` node assembles a list of [sites]({{< ref "siteblock" >}}) from a species, and which can then be iterated over and reference by other nodes (e.g. the `Calculate*` nodes).

## Description

The `Select` node assembles a list of coordinates (and axes, if present) for one or more defined sites. The mechanism of selection can be controlled to provide exclusions with other sites (e.g. based on the parent molecule containing the site) or select only sites within a given distance of another reference point. Iteration over the sites is performed via the `ForEach` branch.

When nesting `Select` nodes in analysis routines, the selection of sites can be carefully controlled in order to avoid unwanted groups of selected nodes. For instance, the `SameMoleculeAsSite` keyword allows another `Select` node to be reference, and which will force the current `Select` node to only select a site if it is present in the same molecule as the other site.

## Branching

The hidden `ForEach` keyword / branch is executed for each site selected by the node, and may contain any other analysis-style nodes, including other `Select` nodes.

## Configuration

### Control Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Site`|`name`<br/>`[name...]`|--|A list of `name`d sites to select.|
|`DynamicSite`|`name`<br/>`[name...]`|--|A list of `name`d [`DynamicSite`]({{< ref "dynamicsitenode" >}})s to select.|
|`SameMoleculeAsSite`|`name`|--|Request that selected sites come from the molecule containing the current site in the `name`d `Select` node.|
|`ExcludeSameMolecule`|`name`<br/>`[name...]`|--|Exclude sites from the selection if they are present in the same molecule as the current site in any of the `name`d `SelectNode`s.|
|`ExcludeSameSite`|`name`<br/>`[name...]`|--|Exclude sites from the selection if they are the current site in any of the `name`d `SelectNode`s.|
|`ReferenceSite`|`name`|--|Site from the `name`d `Select` node to be used as a reference point when determining inclusions / exclusions.|
|`InclusiveRange`|`min`<br/>`max`|--|Distance range from `ReferenceSite` within which sites are selected.|
