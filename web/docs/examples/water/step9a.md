---
title: Step 9a - Centre-of-Mass RDF
type: docs
weight: 12
---


Examining the centre-of-mass RDF between molecules is often useful since it gives a general picture of the arrangement between molecule. A related quantity is the coordination number, i.e. the number of molecules within solvation shells that typically form around molecules in the liquid. Here we'll set up calculation of the centre-of-mass RDF and the associated coordination number for the first solvation shell. Dissolve provides a predefined layer to calculate both at once, however first we need to define a suitable site - the centre of mass of the water molecule.

### Define the Centre-of-Mass Site

If you don't already have a site `COM` representing the centre-of-mass of the molecule, perform the following steps:

{{< action type="tabs" >}}Open the {{< gui-tab text="Water" type="Species" >}} species tab and go to the {{<gui-button icon="site" text="Sites">}} section {{< /action >}}
{{< action type="mouse" text="In the species viewer to the right, click-drag with the left mouse button and draw a box around all three atoms of the water molecule to select them." />}}
{{< action type="mouse" >}}Right-click the selcted atoms and click {{< gui-button text="Create site from" >}} &#8680; {{< gui-button text="Atoms" >}}{{< /action >}}
{{< action type="check" >}}In the {{<gui-button icon="site" text="Sites">}} section, under **Definition** select the **Weight origin by mass** option {{< /action >}}
{{< action type="edit" text="Double-click on the new site in the list and change its name to `COM`" />}}

You'll see the site represented in the species viewer as a 3D crosshair indicating the coordinates of the origin, surrounded by a small cube.

### Create an Analysis Layer

We'll now create one of Dissolve's predefined layers to give us the processing modules we need:

{{< action type="menu" text="Layer &#8680; Create &#8680; Analysis &#8680; RDF & Coordination Number" />}}


The new layer contains the following modules:

| Module | Purpose |
|--------|---------|
| {{< module "SiteRDF" >}} | Calculates the radial distribution function between two specified sites |

We'll need to set up both of these modules to calculate exactly what we need.  First, {{< module "SiteRDF" >}}:

{{< action type="mouse" >}}Select the {{< module "SiteRDF" >}} module and display its {{< gui-button text="Options" icon="options" >}}{{< /action >}}
{{< action type="check" text="Click the **SiteA** option and choose the `COM` site" />}}
{{< action type="check" text="Select the `COM` site for **SiteB** as well" />}}
{{< action type="check" text="Ensure that the **ExcludeSameMolecule** option is enabled" />}}

Since we are calculating the RDF of a site around itself, the **ExcludeSameMolecule** option prevents consideration of the same site with itself (which would give a distance of zero and result in a large undesirable spike at the beginning of the RDF). The distance range over which to calculate the RDF can be set in the **Control** settings group, but the defaults (0 - 10 &#8491; with a step size of 0.05 &#8491;) are fine for what we need.

For the coordination number calculation:

{{< action type="check" text="Tick the **RangeAEnabled** option" />}}
{{< action type="edit" text="Set the maximum for **RangeA** to 3.4 &#8491;" />}}

We can specify up to three separate ranges over which to calculate coordination numbers but we will focus on the first coordination shell in this example.

* * *
{{< button pos="left" text="Previous Step" path="step9/">}}
{{< button pos="right" text="Next Step" path="step9b/">}}
