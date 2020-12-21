---
title: DynamicSite (Node)
linkTitle: DynamicSite
description: Generate a list of sites based on atom type or element
---

{{< htable >}}
| | |
|-|-|
|Context|Analysis|
|Name Required?|No|
|Branches|--|
{{< /htable >}}

## Overview

The `DynamicSite` dynamically constructs a list of simplistic sites for use within a [`Select`]({{< ref "selectnode" >}}) node.

## Description

The `DynamicSite` node generates a list of sites (suitable for use in a [`Select`]({{< ref "selectnode" >}}) node) based only on assigned atom type or element. It is not species/molecule specific.

## Configuration

### Control Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`AtomType`|`type`<br/>`[type...]`|--|List of atom types that will be used to select atoms to form the list of sites.|
|`Element`|`el`<br/>`[el...]`|--|List of chemical elements that will be used to select atoms to form the list of sites.|
