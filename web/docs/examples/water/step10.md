---
title: Step 10 - Running the Analysis
type: docs
weight: 15
---

With our analysis modules set up we are now ready to restart the simulation and begin calculating the quantities we requested - we'll call this our "production run". There are a couple of things to note at this point:

1. The {{< module "EPSR" >}} module / layer should remain enabled during the production run in order to maintain the simulation in its present state.
2. All quantities calculated from analysis modules are averaged automatically over time.
3. Simple quantities (e.g. radial distribution functions) tend to accumulate statistics quickly (in a small number of iterations) while more complicated quantities (e.g. spatial distribution functions) will take much longer to give a useful average.

When you're ready, restart the simulation and we can look at the quantities start to form as the data accumulates. Brief overviews of what to expect are given below.

{{<action type="key" text="Ctrl + R" />}}

### Centre-of-Mass RDF

The {{< module "SiteRDF" >}} module output shows a strong peak at around 2.8 &#8491; for the COM&ndash;COM distance in the liquid. Integrating the curve up to the first minimum (approximately 3.4 &#8491;) gives us a first shell coordination number of around 4.7 molecules (displayed in the panel below the graph).

{{< question text="Is the coordination number of 4.7 realistic?" >}}

### Hydrogen Bond Geometry

{{< action type="tabs" >}}Go to the {{< gui-tab text="Analyse HBond" type="Layer" >}} layer {{< /action >}}
{{< action type="mouse" >}}Click the {{< module "DAngle" >}} module and go to the {{< gui-button text="Output" icon="output" >}} page{{< /action >}}

There are three graphs on display here - the B&middot;&middot;&middot;C g(r) (top left), the A&ndash;B&middot;&middot;&middot;C angle histogram (bottom left), and the 2D correlation map of the two (right). The B&middot;&middot;&middot;C g(r) corresponds to the intermolecular H&middot;&middot;&middot;O and you should see the principal hydrogen bonding peak at 1.8 &#8491; followed by a second, broader interaction at 3.3 &#8491;. So, the primary hydrogen bonding distance in the liquid is 1.8 &#8491;, but how directional is this interaction? If we look at the distance-angle map and read off the x axis to 1.8 &#8491; we can see that the A&ndash;B&middot;&middot;&middot;C angle is always between 150 and 180&deg;. In other words, a very directional hydrogen bond! Angles associated to the broader feature at 3.3 &#8491; are around 60&deg;.

{{< question text="What causes the broad feature at feature at 3.3 &#8491; / 60&deg;?">}}

### 3D Liquid Structure

To make sense of the spatial distribution function output by the {{< module "SDF" >}} module we will need to set the central reference molecule to the average molecule we also requested, and adjust the cutoff to get a sensible surface.

{{<action type="key" text="Esc" />}}

{{< action type="tabs" >}}Go to the {{< gui-tab text="Analyse AvgMol/SDF" type="Layer" >}} layer {{< /action >}}
{{< action type="mouse" >}}Click the {{< module "SDF" >}} module and go to the {{< gui-button text="Output" icon="output" >}} page{{< /action >}}
{{< action type="edit" text="Set the **Reference Molecule** to `COM@Water (AvgMol)`" />}}
{{< action type="edit" text="Change the **Lower** cutoff to 0.065" />}}

{{< tip text="Cutoffs for the surface display are given in terms of the number density per cubic &#8491; of the surrounding molecule / site in the simulation box. In the present example we have 1000 water molecules in a cubic box of side length 31.0723 &#8491;, so the number density of water molecules is **1000 &divide; 31.0723<sup>3</sup> = 0.033**. The cutoff we will set below therefore gives a surface that represents twice the bulk density of water molecules, i.e. a higher than normal probability of finding water molecules." />}}

Spatial density functions take a relatively long to accumulate to a point where the surfaces are smooth, but you should be able to make out the salient features after only a handful of iterations. Along each O&ndash;H bond vector there exists a small lobe of density, and represents the positions of molecules acting as hydrogen bond acceptors to the central one (this visually represents the distance and angle we found in the {{< module "DAngle" >}} results above). A halo of density also exists around the oxygen atom, representing the positions of molecules acting as hydrogen bond donors to the central one.


* * *
{{< button pos="left" text="Previous Step" path="step9">}}
{{< button pos="right" text="Back to Index" path="examples/">}}
