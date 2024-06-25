---
title: RotateFragment (Node)
linkTitle: RotateFragment
description: Rotate fragment sites about their local axes
---

{{< htable >}}
| | |
|-|-|
|Context|Analysis, Generation|
|Name Required?|No|
|Branches|--|
{{< /htable >}}

## Overview

The `RotateFragment` node performs a rotation on a fragment site indicated by a {{< node "Select" >}} nodes.

## Description

`RotateFragment` allows the contents of a configuration to be manipulated through rotating fragment sites about one of their local axes.

## Options

### Site

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Axis`|`Axis`|`X`|Local site axis about which to perform the rotation.|
|`Rotation`|[`Expression`]({{< ref "expressions" >}})|`90`|Expression stating the rotation, in degrees, to apply about the specified axis|
|`Site`|{{< node "Select" >}}|--|Site to rotate. Only applicable to fragment sites.|
