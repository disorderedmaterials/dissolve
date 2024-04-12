---
title: Step 2 - Create Configuration
type: docs
weight: 1
---

Of course we need a configuration to use as our calculation / analysis target, and which must represent exactly the data file we're going to read in terms of atom / species ordering, box size etc. The reference system is a 9 mol% isopropanol in water mix, with 3234 molecules - 291 isopropanol followed by 2943 water. The atom coordinates of the xyz species you used in the previous step also reflect the atom ordering within molecules. The periodic box of the reference simulation is cubic with a side length of 48.6080 &#8491;.

So, let's set up a random mix to mirror all of this:


{{< action type="menu" text="Configuration &#8680; Create..." />}}
{{< action type="mouse" >}}Choose both species and press {{< gui-button text="Next" icon="arrowright" >}}{{< /action >}}
{{< action type="mouse" >}}Leave the configuration type as _Mixture_ and press {{< gui-button text="Next" icon="arrowright" >}}{{< /action >}}
{{< action type="mouse" >}}Set the box style to _Fixed Geometry and Size_ and set all three axis lengths **A**, **B**, and **C** to `48.608`, then press {{< gui-button text="Next" icon="arrowright" >}}{{< /action >}}
{{< action type="edit" >}}In the species table change the _Population / Ratio_ of water to `2943` and isopropanol to `291`{{< /action >}}
{{< action type="edit" >}}Set the _Multiplier_ to `1` - this is really important, as otherwise you'll end up with a huge system with around five million molecules in it!{{< /action >}}
{{< action type="mouse" >}}Press {{< gui-button text="Finish" icon="true" >}} to complete the wizard.{{< /action >}}
{{< action type="edit">}}Double-click on the configuration tab's title and change its name to {{<gui-tab type="configuration" text="Bulk">}}{{</action>}}


With this set-up, the {{< node "Parameters" >}} node is in fact redundant since our density is defined by our imposed box volume and the populations of the species. The {{< node "CoordinateSets" >}} nodes also no longer have any purpose since we will be overwriting the configuration coordinates with our reference data, so if you wanted to you could reduce the number of generated sets in those nodes to `1`.

* * *
{{< button pos="left" text="Previous Step" path="step1/">}}
{{< button pos="right" text="Next Step" path="step3/">}}
