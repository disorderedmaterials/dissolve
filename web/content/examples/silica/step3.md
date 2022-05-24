---
title: Step 3 - Set up a Configuration
type: docs
---


To create our silica configuration we'll use the "relative random mix" generator as a basis.

{{< action type="menu" text="Configuration &#8680; Create &#8680; Relative random mix" />}}
{{< step text="Choose both the silicon and the oxygen species and continue" />}}
{{< action type="edit" text="Double-click the new configuration tab and rename it to `Bulk`" />}}


Let's set-up the parameters in the generator first of all. The relative random mix uses the population of the first species (referred to as `A`) as the reference, and sets the population of all other species relative to the first, based on user defined ratios. So, here we will use the silicon species as `A` and set its population explicitly, and set the relative population of species `B` (the oxygen) to be twice that of `A`, thus giving us SiO<sub>2</sub>.

{{< action type="settings">}}Select the {{< gui-node "Parameters" >}} node in the generator{{< /action >}}
{{< action type="edit" text="Change the value of the **populationA** variable to `1500`" />}}
{{< action type="edit" text="Set **ratioB** to `2`" />}}

{{< warn text="If, for some reason, your atomic species are defined the other way round with oxygen appearing first, you'll need to set **populationA** to `3000` and **ratioB** to `0.5`." />}}

The density of bulk, amorphous silica is 0.0664 atoms &#8491;<sup>-3</sup>. We use units of atoms &#8491;<sup>-3</sup> here as this allows us to add both atomic components using the same density value. If we had chosen units of g cm<sup>-3</sup> we would have to know the individual densities of silicon and oxygen in amorphous silica.

{{< action type="edit" text="Change the value of the **rho** variable to `0.0664`." />}}


Finally, the box. We will still use relative lengths and tell Dissolve to scale the box according to our species population and density (the default behaviour), but we will create a monoclinic box rather than a cubic one:

{{< action type="settings">}}Choose the {{< gui-node "Box" >}} node{{< /action >}}
{{< action type="edit" text="Set the angle **&gamma;** to 120 degrees" />}}


We're all set up, so click **Regenerate** to recreate the box using our new parameters. If you zoom the viewer out (mouse wheel) then you should then see a monoclinic box of silica!

* * *
{{< button pos="left" text="Previous Step" path="step2/">}}
{{< button pos="right" text="Next Step" path="step4/">}}
