---
title: Step 9b - Hydrogen Bond Geometry
type: docs
weight: 13
---

Water is the archetypal hydrogen bonding fluid, and so it makes sense to analyse the hydrogen bonding contacts within the fluid. We will be principally interested in the geometry of O&ndash;H&middot;&middot;&middot;O contacts occurring in the liquid in terms of the H&middot;&middot;&middot;O distance and the O&ndash;H&middot;&middot;&middot;O angle.

### Define Atomic Sites

We'll need simple atomic sites for the oxygen and each of the hydrogen atoms in our water molecule, so let's create them now:

{{< action type="tabs" >}}Open the {{< gui-tab text="Water" type="Species" >}} species tab and go to the {{<gui-button icon="site" text="Sites">}} section {{< /action >}}
{{< action type="mouse" text="Click the Oxygen atom in the species viewer to select it" />}}
{{< action type="mouse" >}}Right-click the selcted atom and click {{< gui-button text="Create site from" >}} &#8680; {{< gui-button text="Atoms" >}}. This will create a site called `O`{{< /action >}}
{{< step text="Repeat this for each hydrogen atom individually, naming those sites `H1` and `H2`" />}}

### Create the Analysis Layer

Let's now create a new layer and add the analysis module that we want:

{{< action type="menu" text="Layer &#8680; Create &#8680; Empty" />}}
{{< action type="edit" text="Rename the layer by double-clicking on the tab name, and change it to 'Analyse HBond'" />}}
{{< action type="mouse" >}}Show the module palette for the layer by clicking {{< gui-button icon="palette" text="Show Available Modules" >}} at the very bottom of the module list {{< /action >}}
{{< action type="mouse" >}}Drag a {{< module "DAngle" >}} module over to the **Current Modules** list{{< /action>}}

As its name suggests, the {{< module "DAngle" >}} module calculates distance and angle histograms, and their 2D map, from three target sites. Since the module is general-purpose, we'll need to set up the module to give us information on our specific geometry of interest. {{< module "DAngle" >}} assumes in the target interaction A&ndash;B&middot;&middot;&middot;C that A&ndash;B occur on the same molecule, and gives us a choice as to whether we exclude C sites that are also on the same molecule.

{{< action type="mouse" >}}Select the {{< module "DAngle" >}} module to display its {{< gui-button text="Options" icon="options" >}}{{< /action >}}
{{< action type="edit" text="Change the maximum value of the **DistanceRange** to 5.0 &#8491;" />}}
{{< action type="check" text="Press the button for **SiteA** and choose the `O` site" />}}
{{< action type="check" text="For **SiteB** select both the `H1` and `H2` sites" />}}
{{< action type="check" text="For **SiteC** choose the `O` site" />}}
{{< action type="check" text="Enable the **ExcludeSameMolecule** to ignore interactions where site C is on the same molecule as A and B." />}}

* * *
{{< button pos="left" text="Previous Step" path="step9b/">}}
{{< button pos="right" text="Next Step" path="step9c/">}}
