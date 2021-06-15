---
title: From Scratch
description: Defining a species from scratch
weight: 1
---

There are a couple of basic methods for creating simple species within the Dissolve GUI.

### Atomic Species
Atomic species such as individual ions or noble gas atoms can be created from the main menu:


{{< action type="menu" text="Species &#8680; Create... &#8680; Atomic..." >}}

This brings up a periodic table from which the desired element can be selected. Once created, a suitable atom type must be created and assigned to the species (see [Applying Forcefields]({{< ref "applyff" >}})).

### Drawing by Hand

Dissolve has a species editor tool in which any species can be drawn by hand, although this is only really recommended for relatively simple molecules. From the main menu:

{{< action type="menu" text="Species &#8680; Create... &#8680; Draw..." >}}

The principal role of this species editor is to create a basic species that contains the correct atoms and connectivity (i.e. bonds) between them. A badly-drawn species is not a problem, as one must still [apply a forcefield]({{< ref "applyff" >}}) to it and, once this is done, the geometry can be cleaned up by energy-minimising the species.
