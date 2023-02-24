---
title: Step 9b - Hydrogen Bond Geometry
type: docs
weight: 13
---

Water is the archetypal hydrogen bonding fluid, and so it makes sense to analyse the hydrogen bonding contacts within the fluid. We will be principally interested in the geometry of O&ndash;H&middot;&middot;&middot;O contacts occurring in the liquid in terms of the H&middot;&middot;&middot;O distance and the O&ndash;H&middot;&middot;&middot;O angle.

### Define Atomic Sites

We'll need simple atomic sites for the oxygen and each of the hydrogen atoms in our water molecule, so let's create them now:

{{< action type="tabs" >}}Open the {{< gui-tab text="Water" type="Species" >}} and go to the **Analysis Sites** section {{< /action >}}
{{< action type="mouse" text="Click-drag with the left mouse button in the species viewer and draw a box around the oxygen atom to select it" />}}
{{< step text="Click the **Create** button in the viewer toolbar to create the new site (which will be called `O`)" />}}
{{< step text="Repeat this for each hydrogen atom individually, naming those sites `H1` and `H2`" />}}

### Create the Analysis Layer

Let's now create a new layer and add the analysis module that we want:

{{< action type="menu" text="Layer &#8680; Create &#8680; Empty" />}}
{{< action type="edit" text="Rename the layer by double-clicking on the tab name, and change it to 'Analyse HBond'" />}}
{{< action type="mouse" >}}Show the module palette for the layer by clicking {{< gui-button text="Show Available Modules" >}} at the very bottom of the module list {{< /action >}}
{{< action type="mouse" >}}Drag a {{< gui-module "DAngle" >}} module over to the **Current Modules** list{{< /action>}}

As its name suggests, the {{< gui-module "DAngle" >}} module calculates distance and angle histograms, and their 2D map, from three target sites. Since the module is general-purpose, we'll need to set up the module to give us information on our specific geometry of interest. {{< gui-module "DAngle" >}} assumes in the target interaction A&ndash;B&middot;&middot;&middot;C that A&ndash;B occur on the same molecule, and gives us a choice as to whether we exclude C sites that are also on the same molecule.

{{< action type="mouse" >}}Select the {{< gui-module "DAngle" >}} module to display its options{{< /action >}}
{{< action type="groups" text="Open the **Control** settings group" />}}
{{< action type="edit" text="Change the maximum value of the **DistanceRange** to 5.0 &#8491;" />}}
{{< action type="mouse" text="Press the button for **SiteA** and choose the `O` site" />}}
{{< action type="mouse" text="For **SiteB** select both the `H1` and `H2` sites" />}}
{{< action type="mouse" text="For **SiteC** choose the `O` site" />}}
{{< action type="mouse" text="Enable the **ExcludeSameMolecule** to ignore interactions where site C is on the same molecule as A and B." />}}

* * *
{{< button pos="left" text="Previous Step" path="step9b/">}}
{{< button pos="right" text="Next Step" path="step9c/">}}
