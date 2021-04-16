---
title: Accumulate (Module)
linkTitle: Accumulate
description: Accumulate (average) g(r) and S(Q) partial
---

## Overview

The `Accumulate` takes radial distribution function or structure factor data from a correlation module (such as [`RDF`]({{< ref "rdf" >}}) or [`NeutronSQ`]({{< ref "neutronsq" >}})) and accumulates an online average. As such it can be used to form a smoothed set of partials (and their total) for further analysis, saving to plot in production-quality graphs etc.

The averaging performed by the `Accumulation` module differs from that performed in the correlation modules themselves - there, the average is a moving average, with `N` sets of partials stored in memory or in the restart file. Much like the various `Calculate` modules, the `Accumulation` module forms an online average with no history.

A target module containing partial set data must be provided, along with the type of target data to accumulate (since in the case of the [`NeutronSQ`]({{< ref "neutronsq" >}}), for instance, both g(r) and S(Q) partial sets are generated).

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
