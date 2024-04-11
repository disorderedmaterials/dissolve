---
title: Step 7 - Analyse the Structure
type: docs
---


We'll introduce another analysis module, {{< module "Angle" >}}, to finish off this tutorial. {{< module "Angle" >}} is similar to {{< module "DAngle" >}}, but is more flexible, assumes less about the connectivity between the sites, and gives more comprehensive output. We'll use it to calculate the Si&ndash;O&ndash;Si angle, and the correlations of that angle with the individual Si&ndash;O bonds.

Create an empty layer, and add a {{< module "Angle" >}} module to it.

{{< action type="menu" text="Layer &#8680; Create &#8680; Empty" />}}
{{< action type="mouse">}}Show the module palette for the layer by clicking the {{< gui-button icon="palette" text="Show Available Modules" >}} button at the foot of the module list at the bottom left of the window{{< /action >}}
{{< step >}}Drag a {{< module "Angle" >}} module from the `Analysis` group in the **Module Palette** into the list above to add it to the layer{{< /step >}}

Now let's set it up.

{{< action type="mouse">}}Select the {{< module "Angle" >}} module to display its {{< gui-button text="Options" icon="options" >}}{{< /action >}}
{{< action type="edit" text="Change the **RangeAB** _Min_ and _Max_ values to 1.2 and 2.1 respectively, and corresponding to the range of the Si&ndash;O bonds" />}}
{{< action type="edit" text="Change the **RangeBC** _Min_ and _Max_ values to 1.2 and 2.1 as well" />}}
{{< action type="edit" text="Change the **AngleRange** _BinWidth_ to 5.0" />}}
{{< action type="check" text="Select the `Si` site for `SiteA` and `SiteC`" />}}
{{< action type="check" text="Select the `O` site for `SiteB`" />}}
{{< action type="mouse" text="Enable the **ExcludeSameSiteAC** option, as otherwise we'll get a spike at zero in the resulting angle data" />}}


Now we can start Dissolve running again and look at the calculated data:

{{< action type="key" text="Ctrl-R" />}}
{{< action type="groups" >}}Select the {{< module "Angle" >}} module and open the {{< gui-button text="Output" icon="output" >}} page{{< /action >}}


The three graphs on the first page of {{< module "Angle" >}}s tabs shows the two radial distribution functions A&ndash;B and B&ndash;C, and the A&ndash;B&ndash;C angle. The RDFs should mirror those calculated by the {{< module "NeutronSQ" >}} module (peak at around 1.6 &#8491;) while the normalised angle graph tells us that all the Si&ndash;O&ndash;Si angles are predominantly greater than 120&deg;.

The second and third tabs display us the distance-angle correlation graphs, which are both identical in the present case.


* * *
{{< button pos="left" text="Previous Step" path="step6/">}}
{{< button pos="right" text="Back to Index" path="examples/">}}
