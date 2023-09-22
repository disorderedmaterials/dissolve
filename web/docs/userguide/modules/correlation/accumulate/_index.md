---
title: Accumulate (Module)
linkTitle: Accumulate
description: Accumulate (average) g(r) and S(Q) partial
---

## Overview

The `Accumulate` module takes radial distribution function or structure factor data from a correlation module (such as {{< module "GR" >}} or {{< module "NeutronSQ" >}}) and accumulates an online average. As such it can be used to form a smoothed set of partials (and their total) for further analysis, saving to plot in production-quality graphs etc.

The averaging performed by the `Accumulate` module differs from that performed in the correlation modules themselves - there, the average is a moving average, with `N` sets of partials stored in memory or in the restart file. Much like the various `Calculate` modules, the `Accumulate` module forms an online average with no history.

A target module containing partial set data must be provided, along with the type of target data to accumulate (since in the case of the {{< module "NeutronSQ" >}}, for instance, both g(r) and S(Q) partial sets are generated).

## Options

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Targets`|`Module`...|--|{{< required-label >}}Name of the source module(s) from which to take partial set data to accumulate. If multiple modules are specified then each is accumulated separately, rather than them all being accumulated into one merged dataset.|
|`Data`|`RDF|SQ|OriginalRDF`|`RDF`|Partial set type to take from the target module. Not all partial set types are relevant to all target module types - e.g. `SQ` has no meaning for an {{< module "GR" >}} module, but both `RDF` and `SQ` are relevant for an {{< module "XRaySQ" >}} module. The `OriginalRDF` option is specific to the {{< module "GR" >}} module, and refers to the as-calculated partials before any intramolecular broadening has been applied.

## Export

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Export`|`bool`|`false`|Whether to save accumulated partial set(s) to disk after calculation. A separate file is written for each individual atomic partial between types $i$ and $j$, as well as the summed total.|
