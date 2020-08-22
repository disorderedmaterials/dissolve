---
title: Step 4 - Set up Analysis
---


For our "properties of interest" we'll calculate both the centre-of-geometry RDF and the out-of-plane angle between molecules as a function of distance. We can get both from the [CalculateAxisAngle](../../userguide/modules/calculateaxisangle) module:

| Module | Purpose |
|--------|---------|
| [`CalculateAxisAngle`](../../userguide/modules/calculateaxisangle) | Calculates the distance vs. axis angle map between two sites |

Since our site (`COG`) is set up for the Z axis to be pointing out of the plane of the benzene ring, it is the angle formed between these axes on the different molecules that we'll consider.

Let's add an empty processing layer to to the simulation, add on our module, and set it up:

{{< action type="menu" text="Layer &#8680; Create... &#8680; Empty" >}}
{{< step text="Double-click the layer tab and rename it to `Analysis`" >}}
{{< action type="mouse" text="Show the module palette for the current layer by clicking the **Show Palette** button at the top left of the tab" >}}
{{< action type="mouse" text=" Drag a [`CalculateAxisAngle`](../../userguide/modules/calculateaxisangle) module over to the **Current Modules** list" >}}
{{< step text="Select the [`CalculateAxisAngle`](../../userguide/modules/calculateaxisangle) module to show its options" >}}
{{< action type="groups" text="Open the **Calculation** settings group" >}}
{{< action type="edit" text="Leave the **DistanceRange** settings as-is, but change the Max and Bin Width for the **AngleRange** to 90 and 10 respectively" >}}
{{< action type="groups" text="Open the **Sites** settings group" >}}
{{< step text="Set both **SiteA** and **SiteB** to `COG`">}}
{{< step text="Set the **AxisA** and **AxisB** to `ZAxis`">}}
{{< step text="Enable the **ExcludeSameMolecule** to prevent the unwanted self-correlation spike at 0 in the resulting RDF">}}

{{< tip text="It would also be nice here to calculate the spatial density function (the three-dimensional distribution of molecules around a reference point) and compare it, but this takes more iterations than a (sane) example will allow.">}}


{{< button pos="left" text="Previous Step" path="../step3/">}}
{{< button pos="right" text="Next Step" path="../step5/">}}
