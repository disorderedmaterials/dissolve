---
title: Step 9 - Compare Data
type: docs
---

In the next two sections we'll compare both the centre-of-geometry RDF and the axis angle map between the reference and refined simulations. How you do this is really up to you - you can load any of your restart points into Dissolve to see the data contained within, and then either:

1. Export specific datasets to files and re-plot them side-by-side in your graphing software of choice.
2. Take image snapshots / copy graphs to the clipboard and paste them in a document elsewhere.
3. Run two copies of Dissolve, and load different restart points into each, and compare "interactively".

In the following sections we've exported the basic 1D curves and plotted them in external software, and we compare the 2D data side-by-side. Your results won't look exactly the same, but should show the same trends.

### Centre-of-mass Radial Distribution Functions

We'll begin with the centre-of-mass (ring centre) RDFs.

{{< cimage src="../rdf-comparison.png" caption="Equilibrated and refined centre-of-mass RDFs" >}}

As you can see the differences are quite subtle, but they are there. The refined (red) simulation shows a decrease in the main peak intensity, and an increase in probability of finding molecules at the shortest contact distances (around 4 &#8491;). Looking at the probabilities between z-axis angles we observe that "flatter" angles are more likely in the refined simulation:

{{< cimage src="../angle-comparison.png" caption="Equilibrated and refined Z-axis angle distributions" >}}

These functions only give us a hint at what the structural changes in the liquid are, however, and we need to combine them with the distance-angle map.

### Distance/Angle Correlations

{{< cimage src="../dangle-comparison.png" caption="Equilibrated and refined distance/angle maps" >}}

The 2D function calculated by {{< gui-module "CalculateAxisAngle" >}} shows us variations in the centre-of-mass RDF with z-axis angle - the key feature is at the lowest angles (tending towards zero, at the bottom of the picture). The refined data on the right shows the emergence of a clear shoulder at 4 &#8491; - as discussed in the [paper by Headen _et al._](https://dx.doi.org/10.1021/ja909084e) this corresponds to the presence of offset, parallel molecule stacking (see Figure 9 in that paper) which is minimal in the reference simulation. While it might be argued that this is a rather subtle change, it's important to bear in mind that a typical off-the-shelf forcefield does not show this feature, and it only appears in a meaningful way when we refine against real-world, experimental data.

* * *
{{< button pos="left" text="Previous Step" path="step8/">}}
{{< button pos="right" text="Back to Index" path= "examples/">}}
