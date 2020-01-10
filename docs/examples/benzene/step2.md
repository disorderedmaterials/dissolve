---
title: Step 2 - Set up a Configuration
parent: Comparing Benzene Structure
grand_parent: Examples
---
# Comparing Benzene Structure

## 2. Set up a Configuration

Time to create a suitable liquid configuration for our simulation using the predefined "simple random mix" generator:

> Configuration &#8680; Create... &#8680; Simple random mix
{: .action .action_menu}
> Select the benzene species and continue
{: .step}

In the interests of speed, we'll use the default number of molecules (100) for the simulation, but we will need to set the correct density. We'll use chemical units of g/cm<sup>3</sup> to specify it:

> Open the settings for the `Parameters` node
{: .action .action_settings}
> Check that the value of the "rho" variable is 0.876. Note that this is "just a number" - units are given in the `AddSpecies` node when we reference this value.
{: .step}
> Open the settings for the `AddSpecies` node
{: .action .action_settings}
> Make sure that the units for the **Density** are set to `g/cm3` and that the equation simple references our parameter `rho`.
{: .step}
> Click _Regenerate_{: .text-green-100} to recreate the configuration with our adjusted parameters.
{: .action .action_mouse}

The resulting box will have a side length of just over 24.5 &#8491;.


Before we move on, we will have to reduce our pair potential range from the default of 15 &#8491; since it is too large for our system:

> **Forcefield** tab, **Pair Potentials** section
{: .action .action_tabs}
> Reduce the pair potential **Range** from 15 to 12 &#8491;
{: .step}


![](benzenebox.png){: .img-centre}
*Initial, randomised benzene box containing 100 molecules*

[Previous Step](step1.md){: .btn }   [Next Step](step3.md){: .btn .right}
