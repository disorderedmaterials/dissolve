---
title: Step 1a - Draw the Molecule
type: docs
weight: 2
---


We will draw our molecule using Dissolve's built-in editor:

{{< action type="menu" text="Species &#8680; Create &#8680; Draw..." />}}


A new editor window opens in which we can create our new species.

{{< action type="mouse" text="Choose the drawing mode ![](/img/viewer_edit.png) from the toolbar above the species viewer" />}}
{{< action type="edit" >}}Change the drawing element to oxygen - the current drawing element is represented on the button next to the drawing mode tool, defaulting to {{< gui-button text="C" >}}, so click the button to bring up a periodic table for you to select a new element from {{< /action >}}
{{< action type="mouse" text="Left-click somewhere in the viewer below to place an oxygen atom" />}}
{{< action type="edit" text="Change the drawing element to hydrogen" />}}
{{< action type="mouse" text=" Left-click-drag from the existing oxygen atom to draw a hydrogen bound to it" />}}
{{< step text="Repeat for the other hydrogen" />}}
{{< action type="mouse" >}}Click {{< gui-button text="OK" >}} to close the editor and create the new species{{< /action >}}

Finally, let's rename it:

{{< action type="edit" text="Double-click on the new species tab's title and change the name from `NewSpecies` to `Water`" />}}


{{< cimage src="../badwater.png" caption="A very badly drawn water molecule" >}}

You'll now have a water molecule that might look like its gone ten rounds with Mike Tyson, so let's clean it up. First, we'll need to assign suitable forcefield terms. We'll use a water model known as 'SPC/Fw' (Yujie Wu, Harald L. Tepper and Gregory A. Voth, 'Flexible simple point-charge water model with improved liquid-state properties', <i>Journal of Chemical Physics</i> <b>124</b> 024503 (2006), [http://dx.doi.org/10.1063/1.2136877](http://dx.doi.org/10.1063/1.2136877)) which basically means "simple point charge, flexible water".

{{< action type="menu" text="Species &#8680; Add Forcefield Terms..." />}}
{{< step >}} From the available forcefields choose `SPC/Fw` and click {{< gui-button text="Next" icon="arrowright" >}} {{< /step >}}
{{< step >}} We need to assign new atom types for all of our water molecule's atoms - the default option, **Determine atom types for all atoms**, does just that so click {{< gui-button text="Next" icon="arrowright" >}} {{< /step >}}
{{< step >}} There will be no conflicts with existing atom types as there are no atom types already defined, so click {{< gui-button text="Next" icon="arrowright" >}} {{< /step >}}
{{< step >}} For the intramolecular terms we want to assign them and reduce to master terms, which again are the default settings, so hit {{< gui-button text="Next" icon="arrowright" >}} again {{< /step >}}
{{< step >}} Dissolve will check to see if there are naming conflicts between new and existing master terms, just as it did for the atom types. There will be none as, again, we had no master terms to start with, so click {{< gui-button text="Finish" icon="true" >}} to exit the wizard {{< /step >}}

Take a look at the {{<gui-tab text="Forcefield" type="ff">}} section for the species and you'll see that we now have atom types assigned to our atoms, and suitable intramolecular terms assigned to the bonds and angle. Note that the functional forms of the interactions are actually the names of master terms, and are preceded with `@` to distinguish them as such (e.g. `@HW-OW-HW`). Master terms are global and can be referenced by one or more species, and are particularly useful when molecules possess high symmetry as there is no need to repeat the same parameter definitions. Furthermore, as we shall see later, adjusting species geometry by modifying the master terms is much easier than modifying all the individual values within a species.

We can now clean up the geometry of our molecule by doing a quick geometry optimisation.

{{< action type="mouse" >}}Click the {{<gui-button icon="medic" text="medic">}} icon in the species viewer toolbar to optimise the geometry of the molecule{{< /action >}}

* * *
{{< button pos="left" text="Back to Overview" path="">}}
{{< button pos="right" text="Next Step" path="step1b">}}
