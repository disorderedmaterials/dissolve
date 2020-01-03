---
title: Step 1a - Draw the Molecule
parent: Liquid Water
grand_parent: Examples
---
# Liquid Water

## 1a. Draw the Molecule

First, let's create an empty species and give it a suitable name.

> Species &#8680; Create... &#8680; Empty_
{: .action .action_menu}
> Double-click on the new species tab's title and change the name to `Water`
{: .step}

Now we can draw and connect the atoms we need to create our water molecule.

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

![Very bad water](badwater.png){: .img-centre}

You'll now have a water molecule that might look like its gone ten rounds with Mike Tyson, so let's clean it up. First, we'll need to assign suitable forcefield terms.

> Species &#8680; Add Forcefield Terms
{: .action .action_menu}
> From the available forcefields choose `SPC/Fw` and click _Next_{: .text-green-100}
{: .step}
> For the atom types we want to _Assign all from Forcefield_{: .text-green-100}. For the intramolecular terms we want to assign them and reduce to master terms. Leave the settings as they currently are, and click _Next_{: .text-green-100}
{: step}
> There should be no conflicts with existing atom types or master terms, so click _Next_{: .text-green-100} then _Finish_{: .text-green-100} to exit the wizard.
{: .step}

Take a look at the _Contents & Forcefield_{: .text-green-100} section for the species and you'll see that we now have atom types assigned to our atoms, and suitable intramolecular terms assigned to the bonds and angle. Note that the functional forms of the interactions are actually the names of master terms, and are preceded with `@` to distinguish them as such (e.g. `@HW-OW-HW`). Master terms are global and can be referenced by one or more species, and are particularly useful when molecules possess high symmetry as there is no need to repeat the same parameter definitions. Furthermore, as we shall see later, adjusting species geometry by modifying the master terms is much easier than modifying all the individual values within a species.

We can now clean up the geometry of our molecule by doing a quick geometry optimisation.

> Click the "medic" icon in the species viewer toolbar - ![Medic - Geometry optimise the current species](../icons/viewer_medic.png)
{: .action .action_mouse}

Do this a couple of times until there's no further appreciable change to the geometry of the molecule.

[Back to Overview](index.md){: .btn }   [Next Step](step1b.md){: .btn .right}
