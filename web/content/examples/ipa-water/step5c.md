---
title: Step 5c - Spatial Density
type: docs
weight: 1
---

We want to probe the SDF of like IPA molecules, as well as water around the central IPA species in this system. We will use two of the same of Dissolve's predefined Average Molecule & SDF analysis layers to do this:

{{< action type="menu" text="Layer &#8680; Create &#8680; Analysis &#8680; Average Molecule & SDF" />}}

Along with the calculation of the SDF, it is useful to have the actual average geometry of the species around the specified site, and so the layer provides not only the {{< gui-module "CalculateSDF" >}} but the {{< gui-module "CalculateAvgMol" >}}

Let's proceed and set up the two modules to probe the IPA-IPA SDF in the layer. We can name the analysis layer **SDF IPA-IPA**.
First, {{< gui-module "CalculateAvgMol" >}}:

{{< action type="mouse" >}}Select the {{< gui-module "CalculateAvgMol" >}} module to display its options{{< /action >}}
{{< action type="groups" text="Open the **Control** settings group" />}} {{< step text="Set the target **Site** for the average molecule calculation to be IPA-COM" />}}

And now {{< gui-module "CalculateSDF" >}}:

{{< action type="mouse" >}}Select the {{< gui-module "CalculateSDF" >}} module to display its options{{< /action >}}
{{< action type="groups" text="Open the **Control** settings group" />}}
{{< action type="mouse" text="Set the central **SiteA** to IPA-COM" />}}
{{< action type="mouse" text="Set the surrounding **SiteB** to O" />}}

Next we repeat to set-up the **SDF IPA-water** layer, we will use be using both molecules respective Centre-of-mass analysis sites:

{{< action type="mouse" >}}Select the {{< gui-module "CalculateAvgMol" >}} module to display its options{{< /action >}}
{{< action type="groups" text="Open the **Control** settings group" />}} {{< step text="Set the target **Site** for the average molecule calculation to be IPA-COM" />}}

And now {{< gui-module "CalculateSDF" >}}:

{{< action type="mouse" >}}Select the {{< gui-module "CalculateSDF" >}} module to display its options{{< /action >}}
{{< action type="groups" text="Open the **Control** settings group" />}}
{{< action type="mouse" text="Set the central **SiteA** to IPA-COM" />}}
{{< action type="mouse" text="Set the surrounding **SiteB** to COM-w " />}}

{{< button pos="left" text="Previous Step" path="step5b/">}} {{< button pos="right" text="Next Step" path="step6/">}}
