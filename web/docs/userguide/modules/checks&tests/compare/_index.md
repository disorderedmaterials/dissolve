---
title: Compare (Module)
linkTitle: Compare
description: Compare data sets and calculate errors
---

## Overview

The `Compare` module graphs two provided datasets along with a delta function that displays the difference between the two graphs. It allows for the selection of an error calculation type and will compute this error value between the two datasets over the complete and specified ranges.

Two data sets must be provided, from internal Dissolve data or external data from the user's filesystem.

## Options

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Data1D`|[`DataSource`]({{< ref "datasource" >}})|--|Two 1D Datasets for comparison.|

### Error

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`ErrorType`|[`ErrorType`]({{< ref "errortype" >}})|`Euclidean`|Error metric to calculate between the two datasets.|
|`ErrorRange`|`double`<br/>`double`|--|Minumum and maximum range bounds over which to calculate the error metric.|
