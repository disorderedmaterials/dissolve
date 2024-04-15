---
title: Step 1 - Create the Species
type: docs
---


### Draw the Molecule

The first thing we'll do is draw a benzene molecule as best we can:

{{< action type="menu" text="Species &#8680; Create &#8680; Draw..." />}}

A new editor window opens in which we can draw the rough geometry and connectivity for our molecule.

{{< iconaction type="action_edit" text="Select draw mode from the toolbar above the species viewer" />}}
{{< step text="Draw a roughly hexagonal ring of six carbon atoms (carbon is the default drawing element) by left-click-dragging in the viewer" />}}
{{< step text="Change the drawing element from `C` to `H` by clicking on the button next to the draw mode icon" />}}
{{< step text="Connect a single hydrogen atom to each carbon by left-click-dragging from each carbon site" />}}
{{< tip text="You can create bonds by left-click-dragging between two existing atoms." />}}
Your final species will look something like this:
{{< cimage src="../badbenzene.png" caption="A very badly drawn benzene molecule" >}}
{{< step >}} Click {{< gui-button text="OK" >}} to close the editor and create the new species {{< /step >}}
{{< action type="edit">}}Double-click on the {{<gui-tab type="species" text="NewSpecies">}} tab and change its name to {{<gui-tab type="species" text="Benzene">}}{{</action>}}

### Apply a Forcefield

Time to make it a little prettier!  We'll assign a standard forcefield to it, and optimise the geometry:

{{< action type="menu" text="Species &#8680; Add Forcefield Terms..." />}}
{{< step >}} From the available forcefields choose `OPLSAA2005/Aromatics` and click {{< gui-button text="Next" icon="arrowright" >}}{{< /step >}}
{{< tip text="You can filter forcefields by keywords in name and description by using the filter box at the top-right of the forcefield selection controls." />}}
{{< step >}} We will use the default _Determine atom types for all atoms_ option to add atom types for every atom in the species, so click {{< gui-button text="Next" icon="arrowright" >}}{{< /step >}}
{{< step >}} There will be no conflicts with existing atom types as there are no atom types already defined, so click {{< gui-button text="Next" icon="arrowright" >}}{{< /step >}}
{{< step >}} For the intramolecular terms we want to assign them and reduce to master terms (the default settings) so click {{< gui-button text="Next" icon="arrowright" >}} to proceed{{< /step >}}
{{< step >}} There will be no conflicts with existing master terms, so click {{< gui-button text="Finish" icon="true" >}} to exit the wizard {{< /step >}}
{{< action type="mouse">}}Click the {{<gui-button icon="medic" text="Medic">}}icon in the species viewer toolbar to optimise the geometry of the molecule using the forcefield we've just applied{{</action>}}


We will also get ahead here and edit the master terms to reflect the geometry observed in the experimental data, since the forcefield we've applied here doesn't get things quite right.

{{< action type="tabs" >}} Go to the {{< gui-tab type="ff"  text="Forcefield" >}} tab, {{< gui-button text="Master Terms" >}} section{{< /action >}}
{{< action type="edit" text="In the **Bonds** table change the equilibrium bond length parameter (**eq**) of the 'CA-HA' bond term from `1.08` to `1.09` &#8491;" />}}
{{< action type="edit" text="Change the equilibrium bond length of the 'CA-CA' bond term from `1.40` to `1.38` &#8491;" />}}

### Create Deuterated Isotopologue

Since some of the experimental data was measured on deuterated benzene, we'll need to create a suitable C<sub>6</sub>D<sub>6</sub> isotopologue:

{{< action type="tabs" >}} Go to the {{< gui-tab type="species"  text="Benzene" >}} species tab, and click on the {{< gui-button icon="threeSpecies"  text="Isotopologues" >}} section {{< /action >}}
{{< action type="mouse" >}}Click {{< gui-button icon="add" text="Add" >}} to create a new isotopologue for the species{{< /action >}}
{{< action type="edit" text="Change the isotope for the HA atom type from `Natural (bc = -3.739 fm)` to `2 (bc = 6.671 fm)` by clicking on the isotopologue entry and choosing from the drop-down menu" />}}
{{< action type="edit" text="Change the name of the isotopologue to 'C6D6' (double-click on its name to do so)" />}}


The `Natural` isotopologue is always available and doesn't need to be created, but doesn't appear in the list of user-defined isotopologues. It is also not necessary to create "mixed" isotopologues (e.g. for 50:50 mixtures of H:D) as these are created by mixing individual isotopologues.

### Add Analysis Sites

We'll locate our analysis site at the centre of the benzene ring and give it some axes so that we may calculate orientational / spatial functions around it. The figure below shows the atoms we'll select to define the origin (purple), x-axis (red) and y-axis (blue). Using these atoms as reference points for our coordinate system will set the XY plane to that of the ring, with the _z_ axis perpendicular to the ring, pointing out of its center.

{{< cimage src="../site.png" caption="Origin (purple), x-axis (red) and y-axis (blue) atoms defining the oriented benzene site" >}}

{{< action type="tabs" >}} Go to the {{< gui-tab type="species"  text="Benzene" >}} species tab{{< /action >}}
{{< action type="mouse" text="Select all six carbon atoms by shift-clicking on them in the viewer (you may need to click _reset view_ first, to see the whole molecule)" />}}
{{< action type="mouse" >}}Right-click the selected atoms and click {{< gui-button text="Create site from" >}} &#8680; {{< gui-button text="Atoms" >}} This creates a new site with the Carbon atoms as the **Origin Atoms**{{< /action >}}
{{< action type="mouse" >}}Now select the single carbon atom (as depicted in the image above) and right click it.{{< /action >}}
{{< action type="mouse" >}}Click {{< gui-button text="Modify current site" >}} &#8680; {{< gui-button text="Set X-Axis Atoms" >}} to define the direction of the _x_ axis{{< /action >}}
{{< action type="mouse" >}}Finally, shift-click the pair of adjacent carbon atoms and right click on them{{< /action >}} 
{{< action type="mouse" >}}Click {{< gui-button text="Modify current site" >}} &#8680; {{< gui-button text="Set Y-Axis Atoms" >}} to define the direction of the _y_ axis {{< /step >}}
{{< action type="edit" text="Rename the site to `COG` by double-clicking its name" />}}


* * *
{{< button pos="left" text="Back to Overview" path="">}}
{{< button pos="right" text="Next Step" path="step2">}}
