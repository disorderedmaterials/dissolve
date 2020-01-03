---
title: Step 2 - Set up a Configuration
parent: Liquid Water
grand_parent: Examples
---
# Liquid Water

## 2. Set up a Configuration

Time to create a suitable bulk water configuration for our simulation. We'll use the predefined "simple random mix" generator to get us going.

> Configuration &#8680; Create... &#8680; Simple random mix
{: .action .action_menu}
> Choose the water species and continue
{: .step}

We now need to set a few parameters in the generator in order to get the correct setup.

> Open the settings for the `Parameters` node
{: .action .action_settings}
> Check that the value of the "rho" variable is 0.1. This is the default value for the generator, and just so happens to correspond to the density (in atoms &#8491;<sup>-3</sup>) of liquid water at room temperature. The variable will be referenced in the `AddSpecies` node when setting the density of the water species.
{: .step}
> Open the settings for the `AddSpecies` node
{: .action .action_settings}
> Make sure that the units for the **Density** are set to `atoms/A3` and that the equation simple references our parameter `rho`.
{: .step}
> Increase the **Population** to 1000.
{: .step}
> Click _Regenerate_{: .text-green-100} to recreate the configuration with our adjusted parameters.
{: .action .action_mouse}

One thousand water molecules should have a cubic box size of just over 31 Angstroms side length.

Finally, we'll tweak the temperature of the configuration.

> Set the **Temperature** to 298 K
{: .action .action_edit}

![](waterbox.png){: .img-centre}
*Initial, randomised water box containing 1000 molecules*

[Previous Step](step1.md){: .btn }   [Next Step](step3.md){: .btn .right}
