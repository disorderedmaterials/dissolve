---
title: Step 9b - Hydrogen Bond Geometry
parent: Liquid Water
grand_parent: Examples
---
# Liquid Water

## 9b. Hydrogen Bond Geometry

XXX TODO

### Define Atomic Sites

We'll need simple atomic sites for the oxygen and each of the hydrogen atoms in our water molecule, so let's create them now:

> **Water** species tab
{: .action .action_tabs}
> Open the **Analysis Sites** section
{: .step}
> Click-drag with the left mouse button in the species viewer and draw a box around the oxygen atom to select it
{: .action .action_mouse}
> Click the **Create** button in the viewer toolbar to create the new site (which will be called `O`)
{: .step}
> Repeat this for each hydrogen atom individually, naming those sites `H1` and `H2`
{: .step}

### Create the Analysis Layer

Let's now create a new layer and add the analysis module that we want:

> Layer &#8680; Create... &#8680; Empty
{: .action .action_menu}
> Rename the layer by double-clicking on the tab name, and change it to `Analyse HBond`
{: .action .action_edit}
> Show the module palette for the layer by clicking the _Show Palette_{: .text-green-100} button at the top left of the tab.
{: .action .action_mouse}
> Drag a [CalculateDAngle](/userguide/modules/calculatedangle) module over to the **Current Modules** list
{: .step}

As its name suggests, the [CalculateDAngle](/userguide/modules/calculatedangle) module calculates distance and angle histograms, and the associated 2D map, from three target sites. We'll need to set up the module

[Previous Step](step9a.md){: .btn }   [Next Step](step9c.md){: .btn .right}
