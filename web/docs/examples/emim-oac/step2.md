---
title: Step 2 - Set up a Starting Forcefield
type: docs
weight: 1
---

Of course we need a forcefield to describe our ionic liquid, and we'll focus on the widely-used parameters of [Padua and Canongia-Lopes]({{< ref "pcl2019" >}}). In Dissolve this forcefield is implemented in two distinct parts, one covering cations and one covering anions.

### EMIM Cation

{{< action type="tabs">}}Go to the {{< gui-tab type="species" text="EMIM" >}} species tab{{< /action >}}
{{< action type="menu" text="Species &#8680; Add Forcefield Terms..." />}}
{{< step >}} From the available forcefields choose `PCL2019/Cations` and click {{< gui-button text="Next" icon="arrowright" >}}{{< /step >}}
{{< step >}} We will use the default _Determine atom types for all atoms_ option to add atom types for every atom in the species, so click {{< gui-button text="Next" icon="arrowright" >}}{{< /step >}}
{{< step >}} There will be no conflicts with existing atom types as there are no atom types already defined, so click {{< gui-button text="Next" icon="arrowright" >}}{{< /step >}}
{{< step >}} For the intramolecular terms we want to assign them and reduce to master terms (the default settings) so click {{< gui-button text="Next" icon="arrowright" >}} to proceed{{< /step >}}
{{< step >}} There will be no conflicts with existing master terms, so click {{< gui-button text="Finish" icon="true" >}} to exit the wizard {{< /step >}}

Now we have a full description of our cation - if you look on the **Forcefield** section on the species tab you can see that Dissolve has calculated the total charge on the molecule from both atom types and species atoms. Both of these should be `+1`.

{{< tip text="You'll notice that there are warning icons next to the total species atom and atom type charges. These are there only to indicate that the total charge is non-zero - we know that our species are ions, so we ignore these warnings." />}}

### OAc Anion

Now for the anion:

{{< action type="tabs">}}Go to the {{< gui-tab type="species" text="Acetate" >}} species tab{{< /action >}}
{{< action type="menu" text="Species &#8680; Add Forcefield Terms..." />}}
{{< step >}} From the available forcefields choose `PCL2019/Anions` and click {{< gui-button text="Next" icon="arrowright" >}}{{< /step >}}
{{< step >}} We will use the default _Determine atom types for all atoms_ option to add atom types for every atom in the species, so click {{< gui-button text="Next" icon="arrowright" >}}{{< /step >}}

At this point you will see that there is a conflict between one of the atom types Dissolve wants to apply to the OAc species (`HC`) and those that we've just created on the cation. While from a forcefield perspective the atom type `HC` is transferable between the cation and the anion, when we're thinking about refining pair potentials against experimental data it is probably wise to separate it into two types so that we can allow both to vary independently in the refinement process. So:

{{< action type="mouse" text="Select the `HC` entry in the atom types list" />}}
{{< action type="edit">}}Click the {{<gui-button text="Suffix">}} and type in `A`. Click {{<gui-button text="Ok">}} to change the name from `HC` to `HCA` to resolve the naming conflict. {{</action>}}
{{< action type="mouse">}}Click {{< gui-button text="Next" icon="arrowright" >}} to continue{{< /action >}}
{{< step >}} For the intramolecular terms we want to assign them and reduce to master terms (the default settings) so click {{< gui-button text="Next" icon="arrowright" >}} to proceed{{< /step >}}
{{< step >}}Click {{< gui-button text="Finish" icon="true" >}} to exit the wizard {{< /step >}}

* * *
{{< button pos="left" text="Previous Step" path="step1/">}}
{{< button pos="right" text="Next Step" path="step3/">}}
