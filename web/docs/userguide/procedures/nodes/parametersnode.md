---
title: Parameters (Node)
linkTitle: Parameters
description: Store numerical reference parameters
---

{{< htable >}}
| | |
|-|-|
|Context|Any|
|Name Required?|No|
|Branches|--|
{{< /htable >}}

## Overview

The `Parameters` node stores zero or more named numerical values that may be referenced in [expressions]({{< ref "expressions" >}}) within nodes contained in the same encompassing [procedure]({{< ref "procedures" >}}).

## Description

The `Parameters` node stores named values, with each value being one of two explicit types - integer or floating point. The type of the value is automatically detected on input.

## Options

### Data

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Parameter`|`name`<br/>`value`|--|Define a named parameter with the specified `value`, which can be either integer or floating point.|
