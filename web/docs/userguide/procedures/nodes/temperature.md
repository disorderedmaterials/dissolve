---
title: Temperature (Node)
linkTitle: Temperature
description: Set the temperature of the configuration
---

{{< htable >}}
| | |
|--------|----------|
|Context|Generation|
|Name Required?|No|
|Branches|--|
{{< /htable >}}

## Overview

The `Temperature` node simply sets the temperature of the configuration, in Kelvin, which is used by other nodes where relevant (e.g. in evolution algorithms). It is provided as a convenience node to allow programmatic setting of the temperature by other means.

## Options

### Target

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Temperature`|`Expression`|[`Expression`]({{< ref expressions >}})|`300.0`|Temperature, in Kelvin, to apply to the configuration.|
