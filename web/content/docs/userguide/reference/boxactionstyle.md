---
title: Box Action Styles
description: Box action styles for procedure nodes
---

Box action / adjustment styles used by the [`AddSpecies`]({{< ref "addspeciesnode" >}}) procedure node.

|Keyword|Parameters|Description|
|:---:|:--------:|-----------|
|`None`|--|No modification to the current box size is made.|
|`AddVolume`|--|Add additional volume to the current box, based on the population and density of the species being added. If the box already contains molecules, the centres of geometry of the existing molecules are scaled also.|
|`ScaleVolume`|--|Add additional volume to the current box such that after addition of the specified population of the new species, the overall density of the box is the value requested. If the box already contains molecules, the centres of geometry of the existing molecules are scaled also.|
