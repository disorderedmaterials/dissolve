---
title: Step 2 - Set up a Configuration
type: docs
weight: 1
---

Time to create a suitable liquid configuration for our simulation using the predefined "relative random mix" generator:

{{< action type="menu" text="Configuration &#8680; Create &#8680; Relative random mix" />}}
{{< step >}}Choose both the IPA and water species and press {{< gui-button text="Select" >}}{{< /step >}}
{{< action type="edit" text="Double-click on the configuration tab's title and change its name to `Mixture_9_1`" />}}

Now we need to set the correct density, temperature and number of molecules of each species in our simulation.

{{< action type="settings">}}Open the settings for the {{< gui-node "Parameters" >}} node{{< /action >}}
{{< step text="Set value of the _rho_ variable to `0.0979`, which is the density of a 9:1 water to IPA mixture in units of atoms &#8491;<sup>-3</sup>" />}}
{{< action type="settings" >}}Open the settings for the {{< gui-node "Add" >}} node{{< /action >}}
{{< step text="Make sure that the units for the **Density** are set to atoms/A3 and that the value text is set to _rho_" />}}
{{< action type="edit" text="Change the **Populations** within **Add(IPA)** and **Add(water)** to 50 and 450 respectively" />}}
{{< action type="mouse" >}}Click {{< gui-button text="Generate" >}} to recreate the configuration with our adjusted parameters {{< /action >}}

A 9:1 mixture of water to IPA should have a cubic box size of just over 27 &#8491; side length.

We'll also tweak the temperature of the configuration.

{{< action type="edit" text="Set the **Temperature** to 298 K" />}}

Finally, you'll also need to reduce the pair potential range from the default of 15 &#8491; since this is larger than the half the maximum width of our simulation box.

{{< button pos="left" text="Previous Step" path="step1/">}} {{< button pos="right" text="Next Step" path="step3/">}}
