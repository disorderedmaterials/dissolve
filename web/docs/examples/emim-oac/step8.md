---
title: Step 8 - Analysis Sites
type: docs
weight: 1
---

Time to calculate some properties, but first we need to create some analysis sites on our ions.

#### Cation

To be consistent with the [original study](https://doi.org/10.1021/jp102180q) our focal site on the cation will be at the midpoint of the two nitrogens of the imidazolium:

{{< action type="tabs">}}Go to the {{<gui-tab type="species" text="EMIM">}} species tab, and click on the {{<gui-button icon="site" text="Sites">}} section{{</action>}}
{{< action type="mouse" text="Select both nitrogen atoms by holding down the Shift key and clicking on them in the viewer (you can also draw individual boxes around them)" />}}
{{< action type="mouse">}}Right click the selection and click {{< gui-button text="Create site from" >}} &#8680; {{< gui-button text="Atoms" >}} to create a new site with the origin at the midpoint of the these two atoms{{</action>}}
{{< step text="Now select one of the nitrogens on its own and right click it" />}}
{{< action type="mouse">}}Click {{< gui-button text="Modify current site" >}} &#8680; {{< gui-button text="Set X-Axis Atoms" >}} to define the direction of the _x_ axis{{</action>}}
{{< action type="mouse">}}Finally, select the carbon atom in between the nitrogens and right click it, selecting {{< gui-button text="Modify current site" >}} &#8680; {{< gui-button text="Set Y-Axis Atoms" >}} in the toolbar to define the direction of the _y_ axis{{</action>}}
{{< action type="edit" text="Double-click the site in the list and rename it to `RingNN`" />}}

#### Anion

For the anion we'll use a simple centre-of-mass site:

{{< action type="tabs">}}Go to the {{<gui-tab type="species" text="Acetate">}} species tab, and click on the {{<gui-button icon="site" text="Sites">}} section{{</action>}}
{{< action type="mouse" text="Select all atoms in the acetate (draw a box around everything with the mouse)" />}}
{{< action type="mouse">}}Right click the selection and click {{< gui-button text="Create site from" >}} &#8680; {{< gui-button text="Atoms" >}} to create a new site{{</action>}}
{{< action type="check" text="In the site **Definition** the _Weight by mass_ option" />}}
{{< step text="This locates the site more-or-less on the carbon atom of the carboxylate group" />}}
{{< action type="edit" text="Double-click the site in the list and rename it to `COM`" />}}

* * *
{{< button pos="left" text="Previous Step" path="step7/">}}
{{< button pos="right" text="Next Step" path="step9/">}}
