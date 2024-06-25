---
title: Configuration Tab
description: Configuration tabs
weight: 3
---

A Configuration defines a complete configuration from one or more defined species. Configurations can be created via the _Configuration_ menu, and can be removed from the simulation by clicking the close button associated with the tab.

### Structure

{{< cimage src="../configurationtab.png" caption="Configuration tab showing details of a defined configuration" width=90% >}}

#### Current Box

If configuration contents have been generated, basic details of the current box will be shown here covering the unit cell parameters, density, and atom / molecule counts.

#### Size Factor Scaling

This option can be used to scale the positions of molecules within the box, increasing the distance between them and helping overcome any bad overlaps from the initial configuration generation.

#### Generator

A configuration is created from a [generatore]({{< ref "generators" >}}) allowing it to be reconstructed automatically on demand. The defined procedure can be viewed and edited directly here.