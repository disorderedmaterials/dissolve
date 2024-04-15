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

{{< action type="tabs" >}}Open the {{< gui-tab text="Water" type="Species" >}} species tab and go to the {{<gui-button icon="site" text="Sites">}} section {{< /action >}}
{{< action type="check" text="Make sure the `COM` site we created in **Step 9a** is selected in the list on the left" />}}
{{< action type="mouse" >}}Select the oxygen atom{{< /action >}}
{{< action type="mouse" >}}Right-click the oxygen atom and click {{< gui-button text="Modify current site" >}} &#8680; {{< gui-button text="Set X-Axis Atoms" >}}{{< /action >}}
{{< action type="mouse" >}}Select either one of the hydrogen atoms{{< /action >}}
{{< action type="mouse" >}}Right-click the selected hydrogen atom and click {{< gui-button text="Modify current site" >}} &#8680; {{< gui-button text="Set Y-Axis Atoms" >}}{{< /action >}}

You'll note that, as soon as you pressed the {{< gui-button text="Y Axis" >}} button a set of axes appeared in the site viewer, letting you know that your definition is complete and showing you how your system of axes are oriented.

### Create the Analysis Layer

We will add another of Dissolve's predefined analysis layers to get the modules that we want:

{{< action type="menu" text="Layer &#8680; Create &#8680; Analysis &#8680; Average Molecule & SDF" />}}

Along with the calculation of the SDF, it is useful to have the actual average geometry of the species around the specified site, and so the layer provides the following modules:

| Module | Purpose |
|--------|---------|
| {{< module "AvgMol" >}} | Calculates the average geometry of the parent species around a specified reference site |
| {{< module "SDF" >}} | Calculates the spatial density function of a particular site around a central reference site |

As the name implies, the {{< module "AvgMol" >}} module will provide us with the average species geometry which we can use as a reference when we plot the spatial density function calculated from the {{< module "SDF" >}} module. The {{< module "SDF" >}} module takes two sites as input - the central site A about which the distribution of site B will be calculated. For both modules, the principal (central) site must have a system of axes defined.

Let's proceed and set up the two modules in the layer. First, {{< module "AvgMol" >}}:

{{< action type="mouse" >}}Select the {{< module "AvgMol" >}} module to display its {{<gui-button icon="options" text="Options">}}{{< /action >}}
{{< action type="mouse" text="Find the **Control** settings group, and click the **Site** option." />}}
{{< action type="edit">}}Set the {{<gui-button icon="species" text="Species">}} to **Water** and the {{<gui-button icon="site" text="Site">}} to **COM** for the average molecule calculation (it will be the only one available, since it is the only one which has a set of axes defined){{</action>}}

And now {{< module "SDF" >}}:

{{< action type="mouse" >}}Select the {{< module "SDF" >}} module to display its {{<gui-button icon="options" text="Options">}}{{< /action >}}
{{< action type="check" text="In the **Control** settings group set the central **SiteA** to `COM`" />}}
{{< action type="check" text="Set the surrounding **SiteB** to `O`" />}}

* * *
{{< button pos="left" text="Previous Step" path="step9b/">}}
{{< button pos="right" text="Next Step" path="step10/">}}
