---
title: Step 2 - Set up a Configuration
parent: Bulk Silica
grand_parent: Examples
---
# Bulk Silica

## 2. Set up a Configuration

To create our silica configuration we'll use the "relative random mix" generator as a basis.

> Configuration &#8680; Create... &#8680; Relative random mix
{: .action .action_menu}
> Choose both the silicon and the oxygen species and continue
{: .step}
> Double-click the new configuration tab and rename it to `Bulk`
{: .action .action_edit}

Let's set-up the parameters in the generator first of all. The relative random mix uses the population of the first species (referred to as `A`) as the reference, and sets the population of all other species as a ratio of relative to the first. So, here will use the silicon species as `A` and set its population explicitly, and set the relative population of species `B` (the oxygen) to be twice that of `A`, thus giving us SiO<sub>2</sub>.

> Open the settings for the `Parameters` node
{: .action .action_settings}
> Change the value of the integer `populationA` variable to 1500.
{: .action .action_edit}
> Set `ratioB` to 2
{: .action .action_edit}

The density of bulk, amorphous silica is 0.0664 atoms &#8491;<sup>-3</sup>:

> Change the value of the real `rho` variable to 0.0664.
{: .action .action_edit}

Next, the box. We will still use relative lengths and scale the box according to our species population and density, but we will create a monoclinic box rather than a cubic one:

> Open the settings for the `Box` node
{: .action .action_settings}
> Set the angle _&#945;_ to 120 degrees
{: .action .action_edit}

Finally, we just need to check the ordering of the species in the `AddSpecies` nodes.

> Open the settings for the first `AddSpecies` node
{: .action .action_settings}
> Make sure that the target _Species_ is `Si`
{: .step}
> Open the settings for the second `AddSpecies` node
{: .action .action_settings}
> Make sure that the target _Species_ is `O`
{: .step}

We're all set up, so click **Regenerate** to recreate the box using our new parameters.

[Previous Step](step1.md){: .btn }   [Next Step](step3.md){: .btn .right}
