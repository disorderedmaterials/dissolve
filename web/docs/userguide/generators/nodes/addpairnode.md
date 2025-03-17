---
title: AddPair (Node)
linkTitle: AddPair
description: Insert correlated molecule pair into a box
---

## Overview

The `AddPair` node is a companion to the {{< node "Add" >}} node, but instead operates on a pair of species. The population refers to a population of pairs of species which are added at their current coordinates, then translated / rotated uniformly in order to maintain their relative coordinates.

Periodic species cannot be used by this node, nor can {{< node "CoordinateSets" >}}.

## Description

Options specific to this node are detailed below. Otherwise the behaviour is identical to the {{< node "Add" >}} node (see its documentation for explanations of other parameters).

## Options

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`SpeciesA`|`name`|--|{{< required-label >}} Target species A of the pair to add.|
|`SpeciesB`|`name`|--|{{< required-label >}} Target species B of the pair to add.|

