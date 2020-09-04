---
title: Step 1 - Create the Species
---


### Draw the Molecule

The first thing we'll do is draw a benzene molecule as best we can:

{{< action type="menu" text="Species &#8680; Create... &#8680; Draw" >}}

A new editor window opens in which we can draw the rough geometry and connectivity for our molecule.

{{< iconaction type="viewer_edit" text="Select draw mode from the toolbar above the species viewer" >}}
{{< step text="Draw a roughly hexagonal ring of six carbon atoms (carbon is the default drawing element) by left-click-dragging in the viewer" >}}
{{< step text="Change the drawing element from `C` to `H` by clicking on the button next to the draw mode icon" >}}
{{< step text="Connect a single hydrogen atom to each carbon by left-click-dragging from each carbon site" >}}
{{< step text="Click **OK** to close the editor and create the new species" >}}
{{< action type="edit" text="Double-click on the new species' tab and change its name to `Benzene`" >}}

### Apply a Forcefield

Time to make it a little prettier!  We'll assign a standard forcefield to it, and optimise the geometry:

{{< tip text="You can filter forcefields by keywords in name and description by using the filter box at the top-right of the forcefield selection controls.">}}

{{< action type="menu" text="Species &#8680; Add Forcefield Terms" >}}
{{< step text="From the available forcefields choose `OPLSAA2005/Aromatics` and click **Next**">}}
{{< step text="We will use the default _Determine atom types for all atoms_ option to add atom types for every atom in the species, so click **Next**">}}
{{< step text="There will be no conflicts with existing atom types as there are no atom types already defined, so click **Next**" >}}
{{< step text="For the intramolecular terms we want to assign them and reduce to master terms (the default settings) so click **Next** to proceed">}}
{{< step text="There will be no conflicts with existing master terms, so click **Finish** to exit the wizard">}}
{{< iconaction type="viewer_medic" text="Click the \"medic\" icon in the species viewer toolbar to optimise the geometry of the molecule" >}}


We will also get ahead here and edit the master terms to reflect the geometry observed in the experimental data, since the forcefield we've applied here doesn't get things quite right.

{{< action type="tabs" text="**Forcefield** tab, **Master Terms** section" >}}
{{< step text="Change the bond length (_Parameter 2_) of the `CA-HA` bond term from 1.08 to 1.09 &#8491;">}}
{{< step text="Change the bond length (_Parameter 2_) of the `CA-CA` bond term from 1.40 to 1.38 &#8491;">}}

### Create Isotopologues

Since some of the experimental data was measured on deuterated benzene, we'll need to create a suitable C<sub>6</sub>D<sub>6</sub> isotopologue:

{{< action type="tabs" text="**Benzene** species tab, **Isotopologues** section" >}}
{{< step text="Click **Add**">}}
{{< step text="Expand the entries for the 'NewIsotopologue' by clicking the [+] next to its name, or by clicking **Expand All**">}}
{{< step text="Change the isotope for the HA atom type from `Natural (bc = -3.739 fm)` to `2 (bc = 6.671)` by clicking on the isotopologue entry and chosing from the drop-down menu">}}
{{< step text="Change the name of the isotopologue to 'C6D6' (click on the name to do so)">}}
{{< step text="">}}


The other isotopologues we need are either generated automatically by Dissolve (i.e. the natural isotopologue for C<sub>6</sub>H<sub>6</sub>) or are created by mixing existing isotopologues (as will be the case for the 50:50 mix).

### Add Analysis Sites

We'll locate our analysis site at the centre of the benzene ring and give it some axes so that we may calculate orientational / spatial functions around it. The figure below shows the atoms we'll select to define the origin (purple), x-axis (red) and y-axis (blue). Using these atoms as refence points for our corrodinate system will set the XY plane to that of the ring, with the _z_ axis perpendicular to the ring.

{{< cimage src="site.png" caption="Origin (purple), x-axis (red) and y-axis (blue) atoms defining the oriented benzene site" >}}

{{< action type="tabs" text="**Benzene** species tab, **Analysis Sites** section" >}}
{{< action type="mouse" text="Select all six carbon atoms in the viewer (you may need to click _reset view_ first, to see the whole molecule)" >}}
{{< step text=" Click **Create** in the viewer toolbar to create a new site with the origin at the center between these six atoms" >}}
{{< step text="Now select the single carbon atom and click **X Axis** in the toolbar to define the direction of the _x_ axis">}}
{{< step text="Finally, select the pair of adjacent carbon atoms and click **Y Axis** in the toolbar to define the direction of the _y_ axis" >}}
{{< action type="edit" text="Rename the site to `COG` by double-clicking `C6`" >}}


{{< button pos="left" text="Back to Overview" path="../">}}
{{< button pos="right" text="Next Step" path="../step2">}}

