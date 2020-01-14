---
title: Step 1 - Create the Species
parent: Comparing Benzene Structure
grand_parent: Examples
---
# Comparing Benzene Structure

## 1. Create the Species

### Draw the Molecule

The first thing we'll do is draw a benzene molecule as best we can:

> Species &#8680; Create... &#8680; Draw_
{: .action .action_menu}

A new editor window opens in which we can draw the rough geometry and connectivity for our molecule.

> Choose "Draw mode" from the toolbar above the species viewer - ![Draw mode](../icons/viewer_edit.png)
{: .action .action_mouse}
> Draw a roughly hexagonal ring of six carbon atoms (carbon is the default drawing element) by left-click-dragging in the viewer
{: .step}
> Change the drawing element from `C` to `H` by clicking on the button next to ![Draw mode](../icons/viewer_edit.png)
{: .step}
> Connect a single hydrogen atom to each carbon by left-click-dragging from each carbon site 
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
> Double-click on the new species' tab and change its name to `Benzene`
{: .action .action_edit}

### Apply a Forcefield

Time to make it a little prettier!  We'll assign a standard forcefield to it, and optimise the geometry:

> Species &#8680; Add Forcefield Terms
{: .action .action_menu}
> From the available forcefields choose `OPLSAA2005/Aromatics` and click _Next_{: .text-green-100}
{: .step}
> For the atom types we want to _Assign all from Forcefield_{: .text-green-100}. For the intramolecular terms we want to assign them and reduce to master terms. Leave the settings as they currently are, and click _Next_{: .text-green-100}
{: step}
> There should be no conflicts with existing atom types or master terms, so click _Next_{: .text-green-100} then _Finish_{: .text-green-100} to exit the wizard.
{: .step}
> Click the "medic" icon in the species viewer toolbar - ![Medic - Geometry optimise the current species](../icons/viewer_medic.png)
{: .action .action_mouse}

We will also get ahead here and edit the master terms to reflect the geometry observed in the experimental data, since the forcefield we've applied here doesn't get things quite right.

> **Forcefield** tab, **Master Terms** section
{: .action .action_tabs}
> Change the bond length (_Parameter 2_) of the `CA-HA` bond term from 1.08 to 1.09.
{: .step}
> Change the bond length (_Parameter 2_) of the `CA-CA` bond term from 1.40 to 1.38.
{: .step}

### Create Isotopologues

Since some of the experimental data was measured on deuterated benzene, we'll need to create a suitable C<sub6</sub>D<sub6</sub> isotopologue:

> **Benzene** species tab, **Isotopologues** section
{: .action .action_tabs}
> Click _Add_{: .text-green-100}
{: .step}
> Expand the entries for the 'NewIsotopologue' by clicking the [+] next to its name, or by clicking _Expand All_{: .text-green-100}
{: .step}
> Change the isotope for the HA atom type from `Natural (bc = -3.739 fm)` to `2 (bc = 6.671)`
{: .step}
> Change the name of the isotopologue to 'C6D6' (double-click on the name to do so)
{: .step}

### Add Analysis Sites

We'll locate our analysis site at the centre of the benzene ring and give it some axes so that we may calculate orientational / spatial functions around it. The figure below shows the atoms we'll select for the origin (purple), x-axis (red) and y-axis (blue), and will set the XY plane to that of the ring, with the _z_ axis perpendicular to the ring.

![](site.png){: .img-centre}
*Origin (purple), x-axis (red) and y-axis (blue) atoms defining the oriented benzene site*

> **Benzene** species tab, **Analysis Sites** section
{: .action .action_tabs}
> Select all six carbon atoms in the viewer
{: .action .action_mouse}
> Click **Create** in the viewer toolbar to create a new site with these six atoms at the origin
{: .step}
> Now select the single carbon atom and click **X Axis** in the toolbar to define the direction of the _x_ axis
{: .step}
> Finally, select the pair of adjacent carbon atoms and click **Y Axis** in the toolbar to define the direction of the _y_ axis
{: .step}
> Rename the site to `COG`
{: .action action_edit}

[Back to Overview](index.md){: .btn }   [Next Step](step2.md){: .btn .right}
