---
title: Step 9c - Centre-of-Mass Spatial Density
type: docs
weight: 14
---


Radial distribution functions provide a spherically-averaged picture of structural correlations between sites, so when there are strongly directional interactions (such as hydrogen bonds, for instance) or simply where the central molecule of interest lacks symmetry, understanding the distribution of other sites in 3D space around a given molecule type is much more informative. A spatial distribution function or spatial density function (SDF) tells us, in a manner analogous to the radial distribution function, the densities of some object in 3D space relative to the bulk density. In other words, given a central molecule, the SDF tells us where other molecules or sites prefer to locate themselves.

The SDF is an "orientation-dependent" function - rather than averaging spherically around a molecule as in the RDF, the SDF preserves spatial information relative to some reference set of axes related to the local geometry of the central molecule. So, our first task is to define a set of reference axes for our water molecule.

### Set Up Site Axes

We will define axes on our centre-of-mass site. In general, a system of reference axes can be constructed quite easily from the following steps:

1. Define a coordinate that will represent the origin of the axes (i.e. the local coordinate {0,0,0}). In the present example this will be the centre-of-mass of the molecule.
2. Select a group of one or more atoms whose coordinates, when averaged, will define absolutely the direction of the _x_ axis.
3. Select a group of one or more atoms whose coordinates, when averaged, will define the rough direction of the _y_ axis (and more importantly the XY plane), and which will be orthogonalised with respect to _x_ (i.e. made to form a 90&deg; angle).
4. Define the _z_ axis from the cross product of the _x_ and _y_ axes.

When defining a site within Dissolve it is your responsibility to provide atom indices for the origin and the representative _x_ and _y_ directions - Dissolve will do the rest. Thus:

{{< action type="tabs" text="**Water** species tab" >}}
{{< step text="Open the **Analysis Sites** section" >}}
{{< action type="mouse" text="If you don't already have the `COM` site, click-drag with the left mouse button in the species viewer and draw a box around the whole molecule to select it, then press the **Create** button in the toolbar, and rename the new site to `COM`." >}}
{{< step text="Make sure the `COM` site is selected in the list on the left" >}}
{{< action type="mouse" text="Select the oxygen atom and click the **X Axis** button in the viewer toolbar" >}}
{{< action type="mouse" text="Select either one of the hydrogen atoms and click the **Y Axis** button in the viewer toolbar" >}}

You'll note that, as soon as you pressed the **Y Axis** button a set of axes appeared in the site viewer, letting you know that your definition is complete and showing you how your system of axes are oriented.

### Create the Analysis Layer

We will add another of Dissolve's predefined analysis layers to get the modules that we want:

{{< action type="menu" text="Layer &#8680; Create... &#8680; Analysis... &#8680; Average Molecule & SDF" >}}

Along with the calculation of the SDF, it is useful to have the actual average geometry of the species around the specified site, and so the layer provides the following modules:

| Module | Purpose |
|--------|---------|
| [`CalculateAvgMol`]({{< ref "calculateavgmol" >}}) | Calculates the average geometry of the parent species around a specified reference site |
| [`CalculateSDF`]({{< ref "calculatesdf" >}}) | Calculates the spatial density function of a particular site around a central reference site |

As the name implies, the [`CalculateAvgMol`]({{< ref "calculateavgmol" >}}) module will provide us with the average species geometry which we can use as a reference when we plot the spatial density function calculated from the [`CalculateSDF`]({{< ref "calculatesdf" >}}) module. The [`CalculateSDF`]({{< ref "calculatesdf" >}}) module takes two sites as input - the central site A about which the distribution of site B will be calculated. For both module, the principal (central) site must have a system of axes defined.

Let's proceed and set up the two modules in the layer:

{{< action type="mouse" text="Select the `CalculateAvgMol` module to display its options" >}}
{{< action type="groups" text="Open the **Target** settings group" >}}
{{< step text="Set the target **Site** for the average molecule calculation to be `COM` (it will be the only one available, since it is the only one which has a set of axes defined)" >}}
{{< action type="mouse" text="Select the `CalculateSDF` module to display its options" >}}
{{< action type="groups" text="Open the **Sites** settings group" >}}
{{< step text="Set the central **SiteA** to `COM` (again, the only one available)" >}}
{{< step text="Set the surrounding **SiteB** to `O`" >}}

{{< button pos="left" text="Previous Step" path="step9b/">}}
{{< button pos="right" text="Next Step" path="step10/">}}
