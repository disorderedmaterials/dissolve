---
title: Step 4 - Set up Analysis
parent: Comparing Benzene Structure
grand_parent: Examples
---
# Comparing Benzene Structure

## 4. Set up Analysis

For our "properties of interest" we'll calculate both the centre-of-geometry RDF and the out-of-plane angle between molecules as a function of distance. We can get both from the [CalculateAxisAngle](/userguide/modules/calculatedaxisangle) module:

| Module | Purpose |
|--------|---------|
| [`CalculateAxisAngle`](/userguide/modules/calculatedaxisangle) | Calculates the distance vs. axis angle map between two sites |

Since our site (`COG`) is set up for the Z axis to be pointing out of the plane of the benzene ring, it is the angle formed between these axes on the different molecules that we'll consider.

Let's add an empty processing layer to to the simulation, add on our module, and set it up:

> Layer &#8680; Create... &#8680; Analysis... &#8680; Empty
{: .action .action_menu}
> Double-click the layer tab and rename it to `Analysis`
{: .step}
> Show the module palette for the current layer by clicking the _Show Palette_{: .text-green-100} button at the top left of the tab
{: .action .action_mouse}
> Drag a [`CalculateAxisAngle`](/userguide/modules/calculatedaxisangle) module over to the **Current Modules** list
{: .action .action_mouse}
> Select the [`CalculateAxisAngle`](/userguide/modules/calculatedaxisangle) module to show its options
{: .step}
> Open the **Calculation** settings group
{: .action .action_groups}
> Leave the **DistanceRange** settings as-is, but change the maximum and binwodth for the **AngleRange** to 90 and 10 respectively
{: .action .action_edit}
> Open the **Sites** settings group
{: .action .action_groups}
> Set both **SiteA** and **SiteB** to `COG`
{: .step}
> Set the **AxisA** and **AxisB** to `Z`
{: .step}
> Enable the **ExcludeSameMolecule** to prevent the unwanted self-correlation spike at 0 in the resulting RDF
{: .step}

> It would also be nice here to calculate the spatial density function and compare it, but this takes more iterations than a (sane) example will allow.
{: .tip}

[Previous Step](step3.md){: .btn }   [Next Step](step5.md){: .btn .right}
