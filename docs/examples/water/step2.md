---
title: Step 2 - Set up a Configuration
parent: Liquid Water
grand_parent: Examples
---
# Liquid Water

## 2. Set up a Configuration

Time to create a suitable bulk water configuration for our simulation.

> Configuration &#8680; Create... &#8680; Simple random mix
{: .action .action_menu}
> Choose the water species and continue
{: .step}

We now need to set a few parameters in the generator in order to get the correct setup.

> Open the settings for the `Parameters` node
{: .action .action_settings}
> Check that the value of the "rho" variable is 0.1
{: .step}
> Open the settings for the `AddSpecies` node
{: .action .action_settings}
> Check that the units for the **Density** are set to `atoms/A3`
{: .step}
> Increase the **Population** to 1000
{: .step}
> Click _Regenerate_{: .text-green-100}
{: .action .action_mouse}

Our new configuration should have a box size of just over 31 Angstroms side length.

Finally, we'll tweak the temperature of the configuration.

> Set the **Temperature** to 298 K
{: .action .action_edit}

![Water box](waterbox.png){: .img-centre}

[Previous Step](step1.md){: .btn }   [Next Step](step3.md){: .btn .right}
