---
title: Step 9a - Centre-of-Mass RDF
parent: Liquid Water
grand_parent: Examples
---
# Liquid Water

## 9a. Centre-of-Mass RDF

Examining the centre-of-mass RDF between molecules is often useful, as are the coordination numbers of molecules within the shells that typically form from the local structure in the liquid. Dissolve provides a predefined layer to calculate both at once, however first we need to define a suitable site - the centre of mass of the water molecule.

### Define the Centre-of-Mass Site

If you don't already have a site `COM` representing the centre-of-mass of the molecule, perform the following steps:

> **Water** species tab
{: .action .action_tabs}
> Open the **Analysis Sites** section
{: .step}
> Click **[+] Add** to create a new site
{: .step}
> In the species viewer to the right, click-drag with the left mouse button and draw a box around all three atoms of the water molecule to select them.
{: .action .action_mouse}
> Click the **Origin** button in the toolbar of the viewer to set the currently-selected atoms as defining the origin
{: .step}
> Select the **Weight by mass** option in the **Origin Atom(s)** section
{: .step}
> Double-click on the new site in the list and change its name to `COM`
{: .action .action_edit}

You'll see the site represented in the species viewer as a 3D crosshair indicating the coordinates of the origin, surrounded by a small cube. 

### Create an Analysis Layer

We'll now create one of Dissolve's predefined layers to give us the processing modules we need:

> Layer &#8680; Create... &#8680; Analysis... &#8680; RDF & Coordination Number
{: .action .action_menu}

The new layer contains the following modules:

| Module | Purpose |
|--------|---------|
| [`CalculateRDF`](/userguide/modules/calculaterdf) | Calculates the radial distribution function between two specified sites |
| [`CalculateCN`](/userguide/modules/calculatecn) | Calculates a coordination number by summing histogram data calculated by a [`CalculateRDF`](/userguide/modules/calculaterdf) module |

We'll need to set up both of these modules to calculate exactly what we need.  First, [`CalculateRDF`](/userguide/modules/calculaterdf).

> Select the [`CalculateRDF`](/userguide/modules/calculaterdf) module to display its options
{: .action .action_mouse}
> Open the **Sites** settings group
{: .action .action_groups}
> Press the button for **SiteA** and choose the `COM` site
{: .step}
> Select the `COM` site for **SiteB** as well
{: .step}
> Enable the **ExcludeSameMolecule** option
{: .step}

Since we are considering the RDF of one type of site around itself, the **ExcludeSameMolecule** option prevents consideration of the same site with itself (which would give a distance of zero and result in a large undesirable spike at the beginning of the RDF). The distance range over which to calculate the RDF can be set in the **Calculation** settings group, but the defaults (0 - 10 &#8491; with a step size of 0.05 &#8491;) are fine for what we need.

Now the [`CalculateCN`](/userguide/modules/calculatecn) module:

> Select the [`CalculateCN`](/userguide/modules/calculatecn) module to display its options
{: .action .action_mouse}
> Open the **Target / Ranges** settings group
{: .action .action_groups}
> Set the maximum for **RangeA** to 3.4
{: .step}

The [`CalculateCN`](/userguide/modules/calculatecn) module calculates up to three separate coordination numbers over different distance regions of the target RDF, but we will focus on the first coordination shell in this example. The target RDF is taken from a specific [`CalculateRDF`](/userguide/modules/calculaterdf) module, indicated by the **SourceRDF** keyword (this has been automatically set).

[Previous Step](step9.md){: .btn }   [Next Step](step9b.md){: .btn .right}
