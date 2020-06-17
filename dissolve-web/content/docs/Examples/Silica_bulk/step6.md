---
title: Step 6 - Analyse the Structure
parent: Bulk Silica
grand_parent: Examples
---
# Bulk Silica

## 6. Analyse the Structure

We'll introduce another anlaysis module, [`CalculateAngle`](../../userguide/modules/calculateangle), to finish off this tutorial. [`CalculateAngle`](../../userguide/modules/calculateangle) is similar to [`CalculateDAngle`](../../userguide/modules/calculatedangle), but is more flexible, assumes less about the connectivity between the sites, and gives more comprehensive output. We'll use it to calculate the Si&ndash;O&ndash;Si angle, and the correlations of that angle with the individual Si&ndash;O bonds.

Create an empty layer, and add a copy of the [`CalculateAngle`](../../userguide/modules/calculateangle) module to it.

> Layer &#8680; Create... &#8680; Empty
{: .action .action_menu}
> Show the module palette for the layer by clicking the _Show Palette_{: .text-green-100} button at the top left of the tab
{: .action .action_mouse}
> Drag a [`CalculateAngle`](../../userguide/modules/calculateangle) module from the `Analysis` group in the _Module Palette_{: .text-green-100} on the left
{: .step}

Now let's set it up.

> Select the [`CalculateAngle`](../../userguide/modules/calculateangle) module to display its options
{: .action .action_mouse}
> Open the **Calculation** settings group
{: .action .action_groups}
> Change the **RangeAB** _Min_ and _Max_ values to 1.2 and 2.1 respectively, and corresponding to the range of the Si&ndash;O bonds
{: .action .action_edit}
> Change the **RangeBC** _Min_ and _Max_ values to 1.2 and 2.1 as well
{: .action .action_edit}
> Change the **AngleRange** _BinWidth_ to 5.0
{: .action .action_edit}
> Open the **Sites** settings group
{: .action .action_groups}
> Change **SiteB** to be the one located on the oxygen, and change **SiteA** and **SiteC** to be the one located on the silicon
{: .step}
> Tick the **ExcludeSameSiteAC** option, as otherwise we'll get a spike at zero in the resulting angle data
{: .step}

Let's open up the [`CalculateAngle`](../../userguide/modules/calculateangle) module in a separate tab so that we can see the graphs for the calculated properties, and start Dissolve running once more.

> **Analysis** layer tab
{: .action .action_tabs}
> Double-click the [`CalculateAngle`](../../userguide/modules/calculateangle) module to open it in a new tab
{: .action .action_mouse}
> Ctrl-R
{: .action .action_key}

The three graphs on the first page of [`CalculateAngle`](../../userguide/modules/calculateangle)s tabs shows the two radial distribution functions A&ndash;B and B&ndash;C, and the A&ndash;B&ndash;C angle. The RDFs should mirror those calculated by the [`NeutronSQ`](../../userguide/modules/neutronsq) module (peak at around 1.6 &#8491;) while the normalised angle graph tells us that all the Si&ndash;O&ndash;Si angles are predominantly greater than 120&deg;.

The second and third tabs display us the distance-angle correlation graphs, which are both identical in the present case.


{{< button pos="left" text="Previous Step" path= "/docs/examples/silica_bulk/step5/">}}
{{< button pos="right" text="Back to Index" path= "/docs/examples/silica_bulk/">}}
