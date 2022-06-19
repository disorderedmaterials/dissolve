---
title: Step 4 - Set up Analysis
type: docs
---


For our "properties of interest" we'll calculate both the centre-of-geometry RDF and the out-of-plane angle between molecules as a function of distance. We can get both from the {{< gui-module "CalculateAxisAngle" >}} module:

| Module | Purpose |
|--------|---------|
| {{< gui-module "CalculateAxisAngle" >}} | Calculates the distance vs. axis angle map between two sites |

Since our site (`COG`) is set up for the Z axis to be pointing out of the plane of the benzene ring, it is the angle formed between these axes on the different molecules that we'll focus on.

Let's add an empty processing layer to to the simulation, add on our module, and set it up:

{{< action type="menu" text="Layer &#8680; Create &#8680; Empty" />}}
{{< action type="edit" text="Double-click the layer tab and rename it to `Analysis`" />}}
{{< action type="mouse">}}Show the module palette for the current layer by clicking the {{< gui-button icon="general_palette" text="Show Available Modules">}} button at the bottom of the module list on the left{{< /action >}}
{{< action type="mouse">}}Drag a {{< gui-module "CalculateAxisAngle" >}} module up to the module list{{< /action >}}
{{< action type="mouse">}}Select the new {{< gui-module "CalculateAxisAngle" >}} module to show its options{{< /action >}}
{{< action type="groups" text="Open the **Control** settings group" />}}
{{< action type="edit" text="For the **DistanceRange** change the _Bin Width_ to `0.1`" />}}
{{< action type="edit" text="For the **AngleRange** change the _Max_ to `90`"/>}}
{{< action type="edit" text="Enable the **Symmetric** option"/>}}
{{< step text="This will account for the planar symmetry of the benzene ring, and map any calculated angles in the range 90 - 180&deg; back in to 0 - 90&deg;" />}}

Now we must tell the module which sites to use for its calculation:

{{< action type="edit" text="Set both **SiteA** and **SiteB** to `COG`" />}}
{{< action type="edit" text="Set the **AxisA** and **AxisB** to `ZAxis`" />}}
{{< action type="edit" text="Enable the **ExcludeSameMolecule** to prevent the unwanted self-correlation spike at $r$ = 0 in the resulting RDF" />}}

{{< tip text="It would also be nice here to calculate the spatial density function (the three-dimensional distribution of molecules around a reference point) and compare it, but this takes more iterations than a (sane) example will allow." />}}


* * *
{{< button pos="left" text="Previous Step" path="step3/">}}
{{< button pos="right" text="Next Step" path="step5/">}}
