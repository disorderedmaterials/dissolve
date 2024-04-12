---
title: Step 2 - Apply a Forcefield
type: docs
weight: 1
---

Time to tell our Cu-BTC how to move! There are lots of forcefields out there for metal organic frameworks, with varying complexity and functional form. Dissolve implements a few of them for specific cases, and we'll use the one by [Zhao _et al._]({{< ref "zhao2010" >}}).

{{< action type="mouse">}}Click on the {{<gui-tab type="species" text="Cu48O192C288H96">}} species tab{{</action>}}
{{< action type="menu" text="Species &#8680; Add Forcefield Terms..." />}}
{{< step >}} From the available forcefields choose `Zhao2010` and click {{< gui-button text="Next" icon="arrowright" >}}{{< /step >}}
{{< tip text="You can filter forcefields by keywords in name and description by using the filter box at the top-right of the forcefield selection controls." />}}
{{< step >}} We will use the default _Determine atom types for all atoms_ option to add atom types for every atom in the species, so click {{< gui-button text="Next" icon="arrowright" >}}{{< /step >}}
{{< step >}} Here you'll see that there are some conflicts with atom types assigned during the CIF import - we will simply overwrite them with those from the forcefield, so click {{< gui-button text="Next" icon="arrowright" >}}{{< /step >}}
{{< step >}} For the intramolecular terms we want to assign them and reduce to master terms (the default settings) so click {{< gui-button text="Next" icon="arrowright" >}} to proceed{{< /step >}}
{{< step >}} There will be no conflicts with existing master terms, so click {{< gui-button text="Finish" icon="true" >}} to exit the wizard {{< /step >}}

Our forcefield for the MOF is now ready, but because we're only treating the unit cell (rather than a supercell) we need to reduce the pair potential range since the default (15.0 &#8491;) is longer than the half-cell length of the Cu-BTC unit cell ($l$ = 26.3336 &#8491;):

{{< action type="tabs">}}Go to the {{< gui-tab type="ff" text="Forcefield" >}} tab, **Pair Potentials** section{{< /action >}}
{{< action type="edit" >}}In the controls, change the _Range_ from `15.0` to `12.0`{{< /action >}}

* * *
{{< button pos="left" text="Previous Step" path="step1">}}
{{< button pos="right" text="Next Step" path="step3">}}
