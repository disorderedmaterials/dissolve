---
title: Step 1 - Create the Species
type: docs
weight: 1
---

## Draw the IPA molecule

We need to create both the 2-propanol (IPA) molecule and the water molecule as separate species. Initially we will focus on the 2-propanol molecule. This tutorial will be lighter in detail on the process of creating a species in order to allow you to implement you knowledge of Dissolve.

{{< action type="menu" text="Species &#8680; Create &#8680; Draw..." />}}

The 2-propanol structure is comprised of three carbons in an alkyl chain with an alcohol group on the middle carbon. We can rename this species as IPA.

## Apply a Forcefield

{{< action type="menu" text="Species &#8680; Add Forcefield Terms..." />}}
{{< step >}} From the available forcefields choose `OPLSAA2005/Alcohols` and click {{< gui-button text="Next" >}}{{< /step >}}
{{< step >}} We will use the default _Determine atom types for all atoms_ option to add atom types for every atom in the species, so click {{< gui-button text="Next" >}}{{< /step >}}
{{< step >}} There will be no conflicts with existing atom types as there are no atom types already defined, so click {{< gui-button text="Next" >}}{{< /step >}}
{{< step >}} As usual with intramolecular terms we want to assign them and reduce to master terms (the default settings) so click {{< gui-button text="Next" >}} to proceed{{< /step >}}
{{< step >}} There will be no conflicts with existing master terms, so click {{< gui-button text="Finish" >}} to exit the wizard {{< /step >}}
{{< iconaction type="viewer_medic">}}Click the {{< gui-icon "viewer_medic" >}} icon in the species viewer toolbar to optimise the geometry of the molecule using the forcefield we've just applied{{< /action >}}

Now, we need to pause for a second here. We want to be able to "distinguish" our terminal methyl hydrogens and the one on the central carbon, but the forcefield has assigned the same atom type to both. Part of the reason is that some of the experimental data we are referencing was measured on partially-deuterated IPA where these atoms have differing isotopes.

{{< tip >}}There is also the aspect that, during the refinement, we will be generating an additional potential between each unique atom type pair, so more atom types means more 'flexibility' in the gernated potential. However, adding extra atom types in this way can also result in too many partials and too much flexibility in the fit, which is not necessarily a good thing. A good rule of thumb is "only use as many atom types as you need.{{< /tip >}}

To fix this, we can manually add a new atom type and assign it to the hydrogen at the C2 position:

{{< action type="tabs" >}} Go to the {{< gui-tab type="ff" text="Forcefield" >}} tab, **Master Terms** section{{< /action >}}
{{< action type="mouse" text="Select the `HC` atom type to highlight it" />}}
{{< action type="mouse" >}}Click {{< gui-button text="Duplicate" >}} to create a secondary atom type with the same charge and parameters, named `HC01`{{< /step >}}
{{< action type="edit">}}Double-click on the new atom type and change its name to `HT`{{< /action >}}

And now to use it in the species:

{{< action type="tabs" >}} Go to the {{< gui-tab type="species" text="IPA" >}} species tab, **Structure** section {{< /action >}}
{{< action type="mouse" text="Click to select the hydrogen atom directly bonded to the central carbon" />}}
{{< action type="mouse" text="Right click on the atom to bring up the context menu" />}}
{{< action type="menu" text="Set... &#8680; Atom Type &#8680; HT &#8680; Set " />}}

## Create the Isotoplogues

Now you'll need to add two isotopologues using the information in the following table:

|Isotopologue|HC|HT|HO|
|------------|--|--|--|
|IPA-D8      |D |D |D |
|IPA-D6      |D |H |H |

As is normal, the _Natural_ isotopologue is always available and doesn't need to be created, but doesn't appear in the list of user-defined isotopologues.

## Repeat for the Water Species

As the data we are analysing is a mixture of IPA and water we need to also create the water species. Add a new species, draw the water molecule, apply the SPC/Fw forcefield, then create the D<sub>2</sub>O isotopologue. Use steps [1](https://www.projectdissolve.com/examples/water/step1a/) – [1b](https://www.projectdissolve.com/examples/water/step1b/) of the Liquid Water tutorial as a guide if you need to be reminded of this process.


{{< button pos="left" text="Previous Step" path="/">}} {{< button pos="right" text="Next Step" path="step2/">}}
