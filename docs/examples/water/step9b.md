---
title: Step 9b - Hydrogen Bond Geometry
parent: Liquid Water
grand_parent: Examples
---
# Liquid Water

## 9b. Hydrogen Bond Geometry

Water is the archetypal hydrogen bonding fluid, and so it makes sense to analyse the hydrogen bonding contacts within the fluid. We will be principally interested in the geometry of O&ndash;H&middot;&middot;&middot;O contacts occurring in the liquid in terms of the H&middot;&middot;&middot;O distance and the O&ndash;H&middot;&middot;&middot;O angle.

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

As its name suggests, the [CalculateDAngle](/userguide/modules/calculatedangle) module calculates distance and angle histograms, and their 2D map, from three target sites. Since the module is general-purpose, we'll need to set up the module to give us information on our specific geometry of interest. The [CalculateDAngle](/userguide/modules/calculatedangle) assumes in the target interaction A&ndash;B&middot;&middot;&middot;C that A&ndash;B occur on the same molecule, and gives us a choice as to whether we exclude C sites that are also on the same molecule.

> Select the [`CalculateDAngle`](/userguide/modules/calculatedangle) module to display its options
{: .action .action_mouse}
> Open the **Calculation** settings group
{: .action .action_groups}
> Change the maximum value of the **DistanceRange** to 5.0 &#8491;
{: .step}
> Open the **Sites** settings group
{: .action .action_groups}
> Press the button for **SiteA** and choose the `O` site
{: .step}
> For **SiteB** select both the `H1` and `H2` sites
{: .step}
> For **SiteC** choose the `O` site
{: .step}
> Enable the **ExcludeSameMolecule** to ignore interactions where site C is on the same molecule as A and B.
{: .step}

[Previous Step](step9a.md){: .btn }   [Next Step](step9c.md){: .btn .right}
