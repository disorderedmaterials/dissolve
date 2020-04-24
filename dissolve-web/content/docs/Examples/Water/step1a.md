---
title: Step 1a - Draw the Molecule
parent: Liquid Water
grand_parent: Examples
weight: 2
---
# Liquid Water

## 1a. Draw the Molecule

We will draw our molecule using Dissolve's built-in editor:

> Species &#8680; Create... &#8680; Draw_
{: .action .action_menu}

A new editor window opens in which we can create our new species.

> Choose "Draw mode" from the toolbar above the species viewer - ![Draw mode](../icons/viewer_edit.png)
{: .action .action_mouse}
> Change the drawing element from `C` to `O` by clicking on the button next to ![Draw mode](../icons/viewer_edit.png)
{: .step}
> Left-click somewhere in the viewer below to place an oxygen atom
{: .step}
> Change the drawing element to hydrogen
{: .step}
> Left-click-drag from the existing oxygen atom to draw a hydrogen bound to it
{: .step}
> Repeat for the other hydrogen
{: .step}
> Click **OK** to close the editor and create the new species
{: .step}

Finally, let's rename it:

> Double-click on the new species tab's title and change the name to `Water`
{: .step}

![](badwater.png){: .img-centre}
*A very badly drawn water molecule*

You'll now have a water molecule that might look like its gone ten rounds with Mike Tyson, so let's clean it up. First, we'll need to assign suitable forcefield terms. We'll use a water model known as 'SPC/Fw' (Yujie Wu, Harald L. Tepper and Gregory A. Voth, 'Flexible simple point-charge water model with improved liquid-state properties', <i>Journal of Chemical Physics</i> <b>124</b> 024503 (2006), [http://dx.doi.org/10.1063/1.2136877](http://dx.doi.org/10.1063/1.2136877)) which basically means "simple point charge, flexible water".

> Species &#8680; Add Forcefield Terms
{: .action .action_menu}
> From the available forcefields choose `SPC/Fw` and click _Next_{: .text-green-100}
{: .step}
> We need to assign new atom types for all of our water molecule's atoms. The default option, _Determine atom types for all atoms_, does just that so click _Next_{: .text-green-100}.
{: .step}
> There will be no conflicts with existing atom types as there are no atom types already defined, so click _Next_{: .text-green-100}.
{: .step}
>For the intramolecular terms we want to assign them and reduce to master terms, which again are the default settings. Click _Next_{: .text-green-100} to proceed.
{: .step}
> Dissolve will check to see if there are naming conflicts between new and existing master terms, just as it did for the atom types. There will be none as, again, we had no master terms to start with, so click _Finish_{: .text-green-100} to exit the wizard.
{: .step}

Take a look at the _Contents & Forcefield_{: .text-green-100} section for the species and you'll see that we now have atom types assigned to our atoms, and suitable intramolecular terms assigned to the bonds and angle. Note that the functional forms of the interactions are actually the names of master terms, and are preceded with `@` to distinguish them as such (e.g. `@HW-OW-HW`). Master terms are global and can be referenced by one or more species, and are particularly useful when molecules possess high symmetry as there is no need to repeat the same parameter definitions. Furthermore, as we shall see later, adjusting species geometry by modifying the master terms is much easier than modifying all the individual values within a species.

We can now clean up the geometry of our molecule by doing a quick geometry optimisation.

> Click the "medic" icon in the species viewer toolbar - ![Medic - Geometry optimise the current species](../icons/viewer_medic.png)
{: .action .action_mouse}

Do this a couple of times until there's no further appreciable change to the geometry of the molecule.

[Back to Overview](/docs/examples/water/){: .btn }   [Next Step](/docs/examples/water/step1b){: .btn .right}
