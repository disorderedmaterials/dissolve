---
title: Step 7 - Analyse the Structure
type: docs
---


We'll introduce another analysis module, {{< gui-module "CalculateAngle" >}}, to finish off this tutorial. {{< gui-module "CalculateAngle" >}} is similar to {{< gui-module "CalculateDAngle" >}}, but is more flexible, assumes less about the connectivity between the sites, and gives more comprehensive output. We'll use it to calculate the Si&ndash;O&ndash;Si angle, and the correlations of that angle with the individual Si&ndash;O bonds.

Create an empty layer, and add a {{< gui-module "CalculateAngle" >}} module to it.

{{< action type="menu" text="Layer &#8680; Create &#8680; Empty" />}}
{{< action type="mouse">}}Show the module palette for the layer by clicking the {{< gui-button icon="general_palette" text="Show Available Modules" >}} button at the foot of the module list at the bottom left of the window{{< /action >}}
{{< step >}}Drag a {{< gui-module "CalculateAngle" >}} module from the `Analysis` group in the **Module Palette** into the list above to add it to the layer{{< /step >}}

Now let's set it up.

{{< action type="mouse">}}Select the {{< gui-module "CalculateAngle" >}} module to display its options{{< /action >}}
{{< action type="groups" text="Open the **Control** settings group" />}}
{{< action type="edit" text="Change the **RangeAB** _Min_ and _Max_ values to 1.2 and 2.1 respectively, and corresponding to the range of the Si&ndash;O bonds" />}}
{{< action type="edit" text="Change the **RangeBC** _Min_ and _Max_ values to 1.2 and 2.1 as well" />}}
{{< action type="edit" text="Change the **AngleRange** _BinWidth_ to 5.0" />}}
{{< action type="edit" text="Select the `Si` site for `SiteA` and `SiteC`" />}}
{{< action type="edit" text="Select the `O` site for `SiteB`" />}}
{{< action type="edit" text="Enable the **ExcludeSameSiteAC** option, as otherwise we'll get a spike at zero in the resulting angle data" />}}


Now we can start Dissolve running again and look at the calculated data:

{{< action type="key" text="Ctrl-R" />}}
{{< action type="groups" >}}Select the {{< gui-module "CalculateAngle" >}} module and open the {{< gui-button text="Output" icon="general_output" >}} page{{< /action >}}


The three graphs on the first page of {{< gui-module "CalculateAngle" >}}s tabs shows the two radial distribution functions A&ndash;B and B&ndash;C, and the A&ndash;B&ndash;C angle. The RDFs should mirror those calculated by the {{< gui-module "NeutronSQ" >}} module (peak at around 1.6 &#8491;) while the normalised angle graph tells us that all the Si&ndash;O&ndash;Si angles are predominantly greater than 120&deg;.

The second and third tabs display us the distance-angle correlation graphs, which are both identical in the present case.


* * *
{{< button pos="left" text="Previous Step" path="step6/">}}
{{< button pos="right" text="Back to Index" path="examples/">}}
