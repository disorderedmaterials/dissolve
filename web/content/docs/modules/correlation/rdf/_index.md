---
title: RDF (Module)
linkTitle: RDF
description: Calculate radial distribution functions between all atom types
---

## Overview

The `RDF` module calculates a full set of partial radial distribution functions, $g_{ij}(r)$ for each unique pair of atom types $i$ and $j$ present in the target configuration(s).

The `RDF` module is designed to calculate the necessary input for other modules such as [`SQ`]({{< ref "sq" >}}) - there is no facility for calculating other types of radial distribution function, e.g. those between the centres-of-mass of molecules. To calculate such quantities see the [analysis modules]({{< ref "analysis" >}}).

## Description

For a configuration containing $N$ distinct atom types there are $\frac{1}{2}N(N+1)$ unique partial radial distribution functions $g_{ij}(r)$, where $i$ and $j$ are the $i^{th}$ and $j^{th}$ atom type respectively. The standard formalism for a radial distribution function between atom types $i$ and $j$ is

$$ g_{ij}(r, \Delta r) = \frac{n_j(r,\Delta r)}{\frac{4 \pi}{3}[(r+\Delta_r)^3 - r^3]\rho_j} $$

where $g_{ij}(r, \Delta r)$ is calculated for a spherical shell with inner radius $r$ and thickness $\Delta r$, $n_j(r, \Delta r)$ is the number of particles of type $j$ within the shell, and $rho_j$ is the bulk number density of $j$ over the whole configuration. $g_{ij}(r)$ therefore represents the discretised probability of finding an atom of type $j$ at a distance $r$ from the central atom type $i$, relative to the probability of finding one assuming a uniform distribution of $j$ throughout the simulation box.

## Configuration

### Control Keywords
|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Averaging`|`n`|`5`|Number of historical partial sets $n$ to combine into final partials|
|`AveragingScheme`|[`AveragingScheme`]({{< ref "averagingscheme" >}})|`Linear`|Weighting scheme to use when averaging partials|
|`BinWidth`|`delta`|`0.025`|Bin width (spacing in $r$) to use|
|`Range`|`r`|`15.0`|Maximum $r$ to calculate $g(r)$ out to, unless `UseHalfCellRange` is `true`|
|`UseHalfCellRange`|`true|false`|`true`|Whether to use the maximal RDF range possible that avoids periodic images. If `true` then the radius of the inscribed sphere for the configuration box is used as the limit.|
|`IntraBroadening`|[`PairBroadeningFunction`]({{< ref "averagingscheme" >}})|`Gaussian`|Type of broadening to apply to intramolecular $g(r)$|
|`Method`|[`PartialsMethod`]({{< ref "averagingscheme" >}})|`Auto`|Calculation method to use. All available methods give the same results, but are suited to specific sizes of system.|
|`Smoothing`|`n`|`0`|Degree of smoothing $n$ to apply to the calculated $g(r)$, where $2n+1$ controls the length in the applied Spline smooth|

### Test Keywords
|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`InternalTest`|`true|false`|`false`|Perform internal check of calculated partials against a set calculated by a simple unoptimised double-loop|
|`Test`|`true|false`|`false`|Test calculated total and partials against reference data (specified with `TestReference`)|
|`TestData`|`target`<br/>[`Data1DFileAndFormat`]({{< ref "data1dformat" >}})|--|Test target and reference data. The `target` is the internal name of a specific radial distribution function, which will be tested for agreement against the reference data. The `TestData` keyword may be given multiple times in order to test different partials, for instance.|
|`TestThreshold`|`delta`|`1.0e-5`|Test threshold (%error) above which tests against reference data will fail|

### Export Keywords
|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Save`|`true|false`|`false`|Whether to save partials and total functions to disk. Separate files are written for each partial between atom types $i$ and $j$, as well as the total. Files are named after the configuration from which they were calculated.|
