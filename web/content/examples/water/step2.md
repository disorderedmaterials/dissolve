---
title: Step 2 - Set up a Configuration
type: docs
weight: 4
---


Time to create a suitable bulk water configuration for our simulation. We'll use the predefined "simple random mix" generator to get us going.

{{< action type="menu" text=" Configuration &#8680; Create... &#8680; Simple random mix" >}}
{{< step text="Choose the water species and continue" >}}

We now need to set a few parameters in the generator in order to get the correct setup.

{{< action type="settings" text="Open the settings for the `Parameters` node">}}
{{< step text="Check that the value of the \"rho\" variable is 0.1. This is the default value of density set in all of the predefined configuration generators, and just so happens to correspond to the density (in atoms &#8491;<sup>-3</sup>) of liquid water at room temperature. The variable will be referenced in the `AddSpecies` node when setting the density of the water species." >}}
{{< action type="settings" text="Open the settings for the `AddSpecies` node">}}
{{< step text="Make sure that the units for the **Density** are set to `atoms/A3` and that the value is set to `rho`." >}}
{{< step text="Increase the **Population** to 1000." >}}
{{< action type="mouse" text="Click **Regenerate** to recreate the configuration with our adjusted parameters.">}}


One thousand water molecules should have a cubic box size of just over 31 Angstroms side length.

Finally, we'll tweak the temperature of the configuration.

{{< action type="edit" text="Set the **Temperature** to 298 K">}}

{{< cimage src="../waterbox.png" caption="Initial, randomised water box containing 1000 molecules" >}}

{{< button pos="left" text="Previous Step" path="step1b/">}}
{{< button pos="right" text="Next Step" path="step3/">}}
