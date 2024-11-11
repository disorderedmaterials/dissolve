---
title: Configuration Tab
description: Configuration tabs
weight: 3
---

A configuration defines a complete system to simulation from one or more defined species. Configurations can be created via the _Configuration_ menu, and can be removed from the simulation by clicking the close button associated with the tab.

Similar in layout to the [species tab]({{< ref speciestab >}}) basic summary values are displayed at the top-right of the tab, including the total number of molecules and atoms in the configuration, and more detailed information including access to the generator is provided through panels to the left.

{{< cimage src="../configurationtab.png" caption="Configuration tab showing details of a defined configuration" width=90% >}}

#### Generator

A configuration is created from a [generator]({{< ref "generators" >}}) allowing it to be reconstructed automatically on demand. The defined procedure can be viewed and edited directly here, and the box regenerated on request by clicking the {{< gui-button text="Generate" >}} button.

#### Global Potentials

Any global potentials created by relevant nodes in the generator will appear here.

#### Targeted Potentials

Any targeted potentials created by relevant nodes in the generator will appear here.
