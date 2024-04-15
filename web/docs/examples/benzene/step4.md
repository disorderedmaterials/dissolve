---
title: Step 4 - Set up Analysis
type: docs
---


For our "properties of interest" we'll calculate both the centre-of-geometry RDF and the out-of-plane angle between molecules as a function of distance. We can get both from the {{< module "AxisAngle" >}} module:

| Module | Purpose |
|--------|---------|
| {{< module "AxisAngle" >}} | Calculates the distance vs. axis angle map between two sites |

Since our site (`COG`) is set up for the Z axis to be pointing out of the plane of the benzene ring, it is the angle formed between these axes on the different molecules that we'll focus on.

Let's add an empty processing layer to to the simulation, add on our module, and set it up:

{{< action type="menu" text="Layer &#8680; Create &#8680; Empty" />}}
{{< action type="edit" text="Double-click the layer tab and rename it to `Analysis`" />}}
{{< action type="mouse">}}Show the module palette for the current layer by clicking the {{< gui-button icon="palette" text="Show Available Modules">}} button at the bottom of the module list on the left{{< /action >}}
{{< action type="mouse">}}Drag a {{< module "AxisAngle" >}} module up to the module list{{< /action >}}
{{< action type="mouse">}}Select the new {{< module "AxisAngle" >}} module to show its {{< gui-button text="Options" icon="options" >}}{{< /action >}}
{{< action type="edit" text="For the **DistanceRange** change the _Bin Width_ to `10`" />}}
{{< action type="edit" text="For the **AngleRange** change the _Max_ to `90`"/>}}
{{< action type="check" text="Enable the **Symmetric** option"/>}}
{{< step text="This will account for the planar symmetry of the benzene ring, and map any calculated angles in the range 90 - 180&deg; back in to 0 - 90&deg;" />}}

Now we must tell the module which sites to use for its calculation:

{{< action type="edit" text="Set both **SiteA** and **SiteB** to `COG`" />}}
{{< action type="edit" text="Set the **AxisA** and **AxisB** to `ZAxis`" />}}
{{< action type="check" text="Enable the **ExcludeSameMolecule** to prevent the unwanted self-correlation spike at $r$ = 0 in the resulting RDF" />}}

{{< tip text="It would also be nice here to calculate the spatial density function (the three-dimensional distribution of molecules around a reference point) and compare it, but this takes more iterations than a (sane) example will allow." />}}


* * *
{{< button pos="left" text="Previous Step" path="step3/">}}
{{< button pos="right" text="Next Step" path="step5/">}}
