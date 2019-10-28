---
title: Step 1 - Create a Water Molecule
parent: Liquid Water
grand_parent: Examples
---
# Liquid Water

## 1. Create a Water Molecule

### Geometry

We'll create our water molecule species by drawing the atoms. Choose _Species→Create...→Empty_{: .text-green-100} from the main menu to create an empty species. Above the species viewer in the _View / Generate_{: .text-green-100} section of the tab you'll see a toolbar:

- Choose "Draw mode" - ![Draw mode](../icons/viewer_edit.png)
- The button to the right shows the current element we are drawing with (currently carbon). Click it and set the drawing element to oxygen.
- Left-click somewhere in the viewer below to place an oxygen atom.
- Change the drawing element to hydrogen.
- Left-click-drag from the existing oxygen atom to draw a hydrogen bound to it.
- Repeat for the other hydrogen.

![Very bad water](badwater.png){: .img-centre}

You'll now have a water molecule that might look like its gone ten rounds with Mike Tyson, so let's clean it up. To achieve that we can apply a forcefield and do a quick geometry optimisation. Choose _Add Forcefield Terms..._{: .text-green-100} from the _Species_{: .text-green-100} menu, select the `SPC/Fw` forcefield, and click _Next_ through to the end of the wizard. Take a look at the _Contents & Forcefield_{: .text-green-100} section for the species and you'll see that we now have atom types assigned to our atoms, and suitable intramolecular terms assigned to the bonds and angle. When adding forcefield terms in this way the default action is for Dissolve to add global master terms for intramolecular interactions rather than assign them individually. Note that the form of the interactions are actually the names of master terms, and are preceded with `@` to distinguish them as such.

We can now clean up the geometry of our molecule by clicking the "medic" icon - ![Medic - Geometry optimise the current species](../icons/viewer_medic.png). Do this a couple of times until there's no further appreciable change to the geometry of the molecule.

### Isotopologues

Our eventual refinement will depend on three isotopic water samples - H<sub>2</sub>O, D<sub>2</sub>O, and a 50:50 mix of the two, so we'll go to the _Isotopologues_{: .text-green-100} section and define our deuterated analogue now. Add a new isotopologue, give it a nice name, and change the isotope for the "HW" atomtype to `2 (bc = 6.671)`.

[Back to Overview](index.md){: .btn }   [Next Step](step2.md){: .btn .right}
