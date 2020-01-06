---
title: Step 9c - Centre-of-Mass Spatial Density
parent: Liquid Water
grand_parent: Examples
---
# Liquid Water

## 9c. Centre-of-Mass Spatial Density

XXX TODO

### Set Up Site Axes

Since we are calculating a function that depends on the orientation of the central molecule, we need to define axes on our centre-of-mass site. A system of reference axes can be constructed quite easily following these steps:

1. Define a coordinate that will represent the origin of the axes (i.e. the local coordinate {0,0,0}). In the present example this will be the centre-of-mass of the molecule.
2. Select a group of one or more atoms whose coordinates, when averaged, will define absolutely lthe direction of the _x_ axis.
3. Select a group of one or more atoms whose coordinates, when averaged, will define the rough direction of the _y_ axis, and which will be orthogonalised into the proper _y_ axis.
4. Define the _z_ axis from the cross product of the _x_ and _y_ axes.

Your responsibility is to provide atom indices for the origin and the representative _x_ and _y_ directions - Dissolve will do the rest. Thus:

> **Water** species tab
{: .action .action_tabs}
> Open the **Analysis Sites** section
{: .step}
> If you don't already have the `COM` site, click-drag with the left mouse button in the species viewer and draw a box around the whole molecule to select it, then press the **Create** button in the toolbar, and rename the new site to `COM`.
{: .action .action_mouse}
> Make sure the `COM` site is selected in the list on the left
{: .step}
> Select the oxygen atom and click the **X Axis** button in the viewer toolbar
{: .action .action_mouse}
> Select either one of the hydrogen atoms and click the **Y Axis** button in the viewer toolbar
{: .action .action_mouse}

You'll note that, as soon as you pressed the **Y Axis** button a set of axes appeared in the site viewer, letting you know that your definition is complete and showing you where your axes are pointing.

[Previous Step](step9b.md){: .btn }   [Next Step](step10.md){: .btn .right}
