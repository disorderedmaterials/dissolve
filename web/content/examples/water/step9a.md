---
title: Step 9a - Centre-of-Mass RDF
type: docs
weight: 12
---


Examining the centre-of-mass RDF between molecules is often useful since it gives a general picture of the arrangement between molecule. A related quantity is the coordination number, i.e. the number of molecules within solvation shells that typically form around molecules in the liquid. Here we'll set up calculation of the centre-of-mass RDF and the associated coordination number for the first solvation shell. Dissolve provides a predefined layer to calculate both at once, however first we need to define a suitable site - the centre of mass of the water molecule.

### Define the Centre-of-Mass Site

If you don't already have a site `COM` representing the centre-of-mass of the molecule, perform the following steps:

{{< action type="tabs" text="**Water** species tab" >}}
{{< step text="Open the **Analysis Sites** section" >}}
{{< step text="Click **[+] Add** to create a new site" >}}
{{< action type="mouse" text="In the species viewer to the right, click-drag with the left mouse button and draw a box around all three atoms of the water molecule to select them." >}}
{{< step text="Click the **Origin** button in the toolbar of the viewer to set the currently-selected atoms as defining the origin" >}}
{{< step text="Select the **Weight by mass** option in the **Origin Atom(s)** section" >}}
{{< action type="edit" text="Double-click on the new site in the list and change its name to `COM`" >}}

You'll see the site represented in the species viewer as a 3D crosshair indicating the coordinates of the origin, surrounded by a small cube.

### Create an Analysis Layer

We'll now create one of Dissolve's predefined layers to give us the processing modules we need:

{{< action type="menu" text="Layer &#8680; Create... &#8680; Analysis... &#8680; RDF & Coordination Number" >}}


The new layer contains the following modules:

| Module | Purpose |
|--------|---------|
| [`CalculateRDF`]({{< ref "calculaterdf" >}}) | Calculates the radial distribution function between two specified sites |
| [`CalculateCN`]({{< ref "calculatecn" >}}) | Calculates a coordination number by summing histogram data calculated by a [`CalculateRDF`]({{< ref "calculaterdf" >}}) module |

We'll need to set up both of these modules to calculate exactly what we need.  First, [`CalculateRDF`]({{< ref "calculaterdf" >}}).

{{< action type="mouse" text=" Select the `CalculateRDF` module to display its options" >}}
{{< action type="groups" text="Open the **Sites** settings group" >}}
{{< step text="Press the button for **SiteA** and choose the `COM` site" >}}
{{< step text=" Select the `COM` site for **SiteB** as well" >}}
{{< step text="Enable the **ExcludeSameMolecule** option" >}}

Since we are considering the RDF of one type of site around itself, the **ExcludeSameMolecule** option prevents consideration of the same site with itself (which would give a distance of zero and result in a large undesirable spike at the beginning of the RDF). The distance range over which to calculate the RDF can be set in the **Calculation** settings group, but the defaults (0 - 10 &#8491; with a step size of 0.05 &#8491;) are fine for what we need.

Now the [`CalculateCN`]({{< ref "calculatecn" >}}) module:

{{< action type="mouse" text="Select the `CalculateCN` module to display its options" >}}
{{< action type="groups" text="Open the **Target / Ranges** settings group" >}}
{{< step text=" Set the maximum for **RangeA** to 3.4 &#8491;" >}}

The [`CalculateCN`]({{< ref "calculatecn" >}}) module calculates up to three separate coordination numbers over different distance regions of the target RDF, but we will focus on the first coordination shell in this example. The target RDF is taken from a [`CalculateRDF`]({{< ref "calculaterdf" >}}) module, specified by the **SourceRDF** keyword (this was been automatically set to be the [`CalculateRDF`]({{< ref "calculaterdf" >}}) module in the current layer).

{{< button pos="left" text="Previous Step" path="step9/">}}
{{< button pos="right" text="Next Step" path="step9b/">}}
