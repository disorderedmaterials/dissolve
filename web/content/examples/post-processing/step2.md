---
title: Step 2 - Create Configuration
type: docs
weight: 1
---

Of course we need a configuration to use as our calculation / analysis target, and which must represent exactly the data file we're going to read in terms of atom / species ordering, box size etc. The reference system is a 9 mol% isopropanol in water mix, with 3234 molecules - 291 isopropanol followed by 2943 water. The atom coordinates of the xyz species you used in the previous step also reflect the atom ordering within molecules. The periodic box of the reference simulation is cubic with a side length of 48.6080 &#8491;.

So, let's set up a random mix to mirror all of this:

{{< action type="menu" text="Configuration &#8680; Create &#8680; Simple random mix" />}}
{{< step >}}Highlight both the IPA and water species and press {{< gui-button text="Select" >}}{{< /step >}}

Let's now adjust the steps of the generator to reflect what we have in the reference data.

First, we will explicitly set the box size:

{{< action type="settings" >}}Click on the {{< gui-node "Box" >}} node to show its options{{< /action >}}
{{< action type="edit" text="Change the **A**, **B**, and **C** box lengths to `48.6080`" />}}

We want to retain this box size when we add the molecules in, so we need to tell the {{< gui-node "Add" >}} nodes to leave the box alone. We'll set the required populations at the same time:

{{< action type="settings" >}}Click on the {{< gui-node "Add" >}} node for the IPA species show its options{{< /action >}}
{{< action type="edit" text="Change the **BoxAction** to `None`" />}}
{{< action type="edit" text="Change the **Population** to `291`" />}}

And for the water:

{{< action type="settings" >}}Click on the {{< gui-node "Add" >}} node for water{{< /action >}}
{{< action type="edit" text="Change the **BoxAction** to `None`" />}}
{{< action type="edit" text="Change the **Population** to `2943`" />}}

With this set-up, the {{< gui-node "Parameters" >}} node is in fact redundant since our density is defined by our imposed box volume and the populations of the species. The {{< gui-node "CoordinateSets" >}} nodes also no longer have any purpose since we will be overwriting the configuration coordinates with our reference data.

* * *
{{< button pos="left" text="Previous Step" path="step1/">}}
{{< button pos="right" text="Next Step" path="step3/">}}
