---
title: Step 8 - Analysis Sites
type: docs
weight: 1
---

Time to calculate some properties, but first we need to create some analysis sites on our ions.

#### Cation

To be consistent with the [original study](https://doi.org/10.1021/jp102180q) our focal site on the cation will be at the midpoint of the two nitrogens of the imidazolium:

{{< action type="tabs" text="Go to the **1-Ethyl-3-methylimidazolium** species tab, **Analysis Sites** section" />}}
{{< action type="mouse" text="Select both nitrogen atoms by holding down the Shift key and  clicking on them in the viewer (you can also draw individual boxes around them)" />}}
{{< step text=" Click **Create** in the viewer toolbar to create a new site with the origin at the midpoint of the these two atoms" />}}
{{< step text="Now select one of the nitrogens on its own and click **X Axis** in the toolbar to define the direction of the _x_ axis" />}}
{{< step text="Finally, select the carbon atom in between the nitrogens and click **Y Axis** in the toolbar to define the direction of the _y_ axis" />}}
{{< action type="edit" text="Double-click the site in the list and rename it to `RingNN`" />}}

#### Anion

For the anion we'll use a simple centre-of-mass site:

{{< action type="tabs" text="Go to the **Acetate** species tab, **Analysis Sites** section" />}}
{{< action type="mouse" text="Select all atoms in the acetate (draw a box around everything with the mouse)" />}}
{{< step text=" Click **Create** in the viewer toolbar to create the new site" />}}
{{< action type="mouse" text="Enable the _Weight by mass_ option" />}}
{{< step text="This locates the site more-or-less on the carbon atom of the carboxylate group" />}}
{{< action type="edit" text="Double-click the site in the list and rename it to `COM`" />}}

* * *
{{< button pos="left" text="Previous Step" path="step7/">}}
{{< button pos="right" text="Next Step" path="step9/">}}
