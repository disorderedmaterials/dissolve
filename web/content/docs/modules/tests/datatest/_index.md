---
title: DataTest (Module)
linkTitle: DataTest
description: Compare calculated data from modules with external or internal references
---

## Overview

`DataTest` is predominantly used in Dissolve's system tests for checking calculated one-, two-, or three-dimensional properties against external reference data. It also allows comparison between datasets within Dissolve, for example when ensuring the correctness and consistency of two different methods calculating the same thing.

## Configuration

### Control Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Data`|`Module`|--|{{< required-label >}}Name of the source module from which to take partial set data|
|`Target`|`RDF|SQ|OriginalRDF`|`RDF`|Partial set type to take from the target module. Not all partial set types are relevant to all target module types - e.g. `SQ` has no meaning for an [`RDF`]({{< ref "rdf" >}}) module, but both `RDF` and `SQ` are relevant for an [`XRqySQ`]({{< ref "xraysq" >}}) module. The `OriginalRDF` option is specific to the [`RDF`]({{< ref "rdf" >}}) module, and refers to the as-calculated partials before any intramolecular broadening has been applied.

### Export Keywords

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Export`|`true|false`|`false`|Whether to save accumulated partials to disk after calculation. A separate file is written for each individual atomic partial between types $i$ and $j$, as well as the summed total.|
