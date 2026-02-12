---
title: AddOnSphere (Node)
linkTitle: AddOnSphere
description: Add molecules to a box, positioning them on the surface of a sphere
---

{{< htable >}}
| | |
|--------|----------|
|Context|Generation|
|Name Required?|No|
|Branches|--|
{{< /htable >}}

## Overview

The `AddOnSphere` node creates molecule copies of a target species in a box, placing them on the surface of a sphere of specified size. It is therefore useful in the creation of assembled structures such as micelles.

Periodic species cannot be used by this node, nor can {{< node "CoordinateSets" >}}.

## Description

Aside from the geometric constraints placed on the positioning of the molecules the behaviour of the node is similar to the {{> node "Add" >}}. Options specific to this node are detailed below. Otherwise the behaviour is identical to the {{< node "Add" >}} node (see its documentation for explanations of other parameters).

## Options

### Target

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`SpeciesSite`|`name`|--|{{< required-label >}} Target species to add along with the reference site for positional purposes. Note that the site must be unique in the parent species (i.e. it cannot represent a number of dynamic sites) and if orientation is required it must also have axes defined.|
|`Radius`|[`expr`]({{< ref "expressions" >}})|`5.0`|Radius of the sphere upon which to place molecules.|
|`Distributions`|[`PointDistributionStyle`]({{< ref "pointdistributionstyle" >}})|`Fibonacci`|Method of point distribution on the sphere.|
|`Variance`|[`expr`]({{< ref "expressions" >}})|`0.0`|Random variance to apply to generated positions.|
|`Orient`|`bool`|`false`|Whether to orient placed molecules along the surface tangent, using a site axis as a reference.|
|`Axis`|`Axis`|`X`|Axis to use if orienting site.|

