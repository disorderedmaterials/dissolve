---
title: Clustering (Module)
linkTitle: Clustering
description: Calculate and visualise clusters.
---

## Overview

The `Clustering` module allows for the calculation and visualisation of clustering behaviour within a configuration. This is performed as a proximity check using a cut-off value between two sites: SiteA and SiteB. Optionally, directionality may be specified for the case of hydrogen bonding. 

## Options

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Configuration`|`Configuration`|--|{{< required-label >}}Specifies the configuration on which to operate.|

### Sites

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`SiteA`|`Site ...`|--|Specify the first site for the definition.|
|`SiteB`|`Site ...`|--|Specify the second site.|
|`Cut-off`|`double`|`3.0`|Specify the maximum distance which two sites may be apart to be considered part of the same cluster.|
|`Self-clustering`|`boolean`|`false`|If SiteA and SiteB are different, A-A and B-B will contribute to the cluster (as opposed to just A-B).|

There are no restrictions on how sites need to be defined.

### Directional Hydrogen Bonding

Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`DirectionalHBonding`|`boolean`|`false`|Toggles directional bonding based on the specified angle tolerance.|
|`AngleTolerance`|`double`|`20.0`|Specify the maximum angle for which the determined vectors may oppose for the interaction to be valid (degrees).|

Directional hydrogen bonding may be set up easily when using Static or Dynamic site types. Each site must be created from only one atom/element/atom type. This is the origin atom. The module will detect all hydrogens bonded to this origin atom and perform the angle checks. Specifically, the angle check is performed between the SiteA(origin) -> SiteB(origin) vector and the SiteA(origin) -> SiteA(hydrogen) vector. All hydrogens connected to the origin (referred to as intermediaries on this page) will be checked against all SiteB origins within the cut-off. This is performed symmetrically for SiteA and SiteB, though it is not necessary for both sites to have hydrogens bonded to their origins, so long as one does. 

Should a use case extend the capabilities of static/dyanmic sites, fragment sites may be used. These allow more specific site definitions and more general directionality checks due to shedding the requirement for hydrogen to be the intermediary. What this means in principle is any atom in a molecule can be set to the origin, and directionality to another origin can be checked using any other set of atoms on the molecule. Another potential use is if instead of both sites acting as both donors and acceptors, it is of interest for only one site to be a donor despite the acceptor's origin's bonds to hydrogen (which would normally automatically cause it to act as a donor as well). In this case, the acceptor site should be defined as a fragment without a #other group (see below).

Fragment sites require a [NETA connectivity definition]({{< ref neta >}}). Beyond that, there are some specific requirements for using Fragment sites with `Clustering`. A typical clustering compatible NETA defintion for a -X-H group (for some origin element X) is `-X(#origin, -H(n=2, #other))` meaning: we're looking for a bond to an X (which is tagged as the origin) where this X has bonded to it two (n=2) hydrogens which will both be tagged as #other. There are a few things to note here: 
 
    1. The origin atom must be tagged using #origin.  
    2. The intermediary atoms (if any, but two hydrogens in this case) should be tagged with #other.  
    3. The number of these intermediary atoms must be specified using n= (else only one of the branching atoms will be tagged as #other)  
    4. To avoid issues, specify as little as possible to get the correct site(s). For example with water `-O(#origin, -H(#other))` will not include both hydrogens, but `#origin, -H(n=2, #other)` will.

### Export
Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`ExportSize`|`boolean`|`false`|Exports the calculated size distribution to SizeDist.txt.|
|`ExportMass`|`boolean`|`false`|Exports the calculated mass distribution to MassDist.txt.|
|`ExportAnalysis`|`boolean`|`false`|Calculates and exports the radius of gyration - cluster mass and the fractal dimension.|
|`MinGyrationSize`|`integer`|`10`|Sets the lower size limit for radius of gyration calculation and fractal dimension.|

Within the `Visualisation` tab under `Output` there exists one other export related function: `Export View to XYZ`. This is fairly self explanatory; it will write an XYZ file containing the current set of molecules shown in the viewer. This XYZ can then be added back into Dissolve in a new configuration for analysis by other modules if desired, or used however elsewhere.

### A note on coordination numbers

The coordination numbers under the `Visualisation` tab provided by the module tell the average number of connections that a site has, only for sites that are involved in the cluster. With multiple sites per molecule, it is entirely possible that some sites on a molecule are not included in the cluster, despite the molecule participating in the cluster, as sites on the same molecule are not considered clustered with one another.