---
title: Step 3 - Apply Charge Scaling
type: docs
weight: 1
---

At this point we'll use some prior knowledge of the EMIM-OAc system. Many ionic liquid forcefields to make the quite reasonable assumption of integer (i.e. +1 and &ndash;1) charges on the ions, however the polarisation of (or charge-transfer between) ionic liquid ions has been the subject of significant debate in the literature, and various simulation studies show that scaling the ion charges to mimic a "static" charge transfer between ions can yield improvements to calculated quantities such as the diffusion coefficient and ion-ion structure.

We'll apply a charge scaling on the ions following the work in ["Structure and dynamics of 1-ethyl-3-methylimidazolium acetate via molecular dynamics and neutron diffraction"](https://dx.doi.org/10.1021/jp102180q). Therein, the authors scaled charges on the ions by a factor of 0.85 and showed improved agreement with experimental data (neutron, of course!):

{{< action type="tabs" >}}Go to the {{< gui-tab type="species" text="EMIM" >}} species tab{{< /action >}}
{{< action type="menu" text="Species &#8680; Scale Charges..." />}}
{{< action type="edit" >}}Change the scale factor to `0.85`{{< /action >}}

You'll see the scaling reflected in the charges within the table on the species tab's **Forcefield**, and the total _Atomic Charge_ there should now be `+0.85`. We need to do the same for the anion, of course:

{{< action type="tabs" >}}Go to the {{< gui-tab type="species" text="Acetate" >}} species tab{{< /action >}}
{{< action type="menu" text="Species &#8680; Scale Charges..." />}}
{{< action type="edit" >}}Apply the scale factor to `0.85`{{< /action >}}

The final important step is to explicitly request that Dissolve use charges from the species atoms, as the default automatic choice is to use atom type charges wherever possible.

{{< action type="tabs">}} Go to the {{< gui-tab type="ff" text="Forcefield" >}} tab, {{< gui-button text="Pair Potentials" >}} section{{< /action >}}
{{< action type="mouse" text="In the _Charge Source_ controls untick the _Choose Automatically_ option and choose the _Species Atoms_ option below" />}}

If we don't do this then Dissolve will elect to use the charges on the atom types (which are still valid).

* * *
{{< button pos="left" text="Previous Step" path="step2/">}}
{{< button pos="right" text="Next Step" path="step4/">}}
