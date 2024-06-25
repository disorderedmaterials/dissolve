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

## Available Parameters

The following parameters are exported by the node:

|Parameter|Description|
|:--------|-----------|
|`nSelected`|The number of sites currently selected by the node|
|`index`|The index of the current site in the ForEach loop. Note that this index is just the "local" index for the loop, and does not have any relation to the indices of the sites per se.| 
|`stackIndex`|The "stack" index of the current site in the ForEach loop. This reflects the ordering of the sites as they are calculated from the configuration and so are tied to the atom indexing.|
`siteIndex`|The "global stack" index of the current site over all possible sites targeted by the selection.|

Thus, if there are _N_ sites of a given type, the `index` for a given site will change based on how many of the _N_ are selected according to other criteria (i.e. within a certain distance, not on the same molecule etc.) while a given site's `stackIndex` will remain the same regardless of how many are selected, provided _N_ does not change (i.e. the population of molecules in the `Configuration` is static).

Similarly, the `siteIndex` remains constant for a given site regardless of how many species sites are requested for selection. So, if three sites are provided having _N_, _M_, and _O_ sites respectively, then the `siteIndex` number goes from `1...(N+M+O)`, with sites from _N_ always being numbered `1...N`, sites from _M_ being numbered `N+1...N+M`, and sites from _O_ being numbered `N+M+1...N+M+O`.

## Options

### Sites

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Site`|`name`<br/>`[name...]`|--|A list of named sites to select.|

### Control

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`SameMoleculeAsSite`|`name`|--|Request that selected sites come from the molecule containing the current site in the named `Select` node.|
|`ExcludeSameMolecule`|`name`<br/>`[name...]`|--|Exclude sites from the selection if they are present in the same molecule as the current site in any of the named `SelectNode`s.|
|`ExcludeSameSite`|`name`<br/>`[name...]`|--|Exclude sites from the selection if they are the current site in any of the named `SelectNode`s.|
|`ReferenceSite`|{{< node "Select" >}}|--|Site from the named `Select` node to be used as a reference point when determining inclusions / exclusions.|
|`InclusiveRange`|`min`<br/>`max`|--|Distance range from `ReferenceSite` within which sites are selected.|
