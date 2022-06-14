---
title: Step 5b - Centre-of-mass Analysis
type: docs
weight: 1
---

For the sake of time, we will only analyse the centre-of-mass molecule-molecule radial distribution functions for IPA-Water and IPA-IPA. We'll add a separate analysis layer for each.

{{< action type="menu" text="Layer &#8680; Create &#8680; Analyse RDF/CN" />}}

We need to set up both the {{< gui-module "CalculateRDF" >}}	and {{< gui-module "CalculateCN" >}} modules, to calculate exactly what we need. First, {{< gui-module "CalculateRDF" >}}:

{{< action type="mouse" >}}Select the {{< gui-module "CalculateRDF" >}} module to display its options{{< /action >}}
{{< action type="groups" text="Open the **Control** settings group" />}}
{{< action type="mouse" text="Press the button for **SiteA** and choose the `COM-IPA` site" />}}
{{< action type="mouse" text="Select the `COM-w` site for **SiteB**" />}}

Since here we are not calculating the RDF of a site around itself, we do not need to **ExcludeSameMolecule** but it is necessary to enable this when setting up the IPA-IPA, or any other same molecule, calculations.
The distance range over which to calculate the RDF can be set in the **Control** settings group, but the defaults (0 - 10 &#8491; with a step size of 0.05 &#8491;) are fine for what we need.

Now the {{< gui-module "CalculateCN" >}} module:

{{< action type="mouse" >}}Select the {{< gui-module "CalculateCN" >}} module to display its options{{< /action >}}
{{< action type="groups" text="Open the **Target / Ranges** settings group" />}}
{{< step text=" Set the maximum for **RangeA** to `5.7` &#8491;" />}}

The {{< gui-module "CalculateCN" >}} module calculates up to three separate coordination numbers over different distance regions of the target RDF, but we will focus on the first coordination shell in this example. The target RDF is taken from a {{< gui-module "CalculateRDF" >}} module, specified by the **SourceRDF** keyword (this will have been set automatically to the {{< gui-module "CalculateRDF" >}} module in the current layer).

Using the previous guidance you should be able to set up a similar layer to calculate IPA-IPA interactions, for this it is advised to use a maximum **RangeA** of 7.2 &#8491;.
If you wanted to also set up water-water and water-IPA analysis layers, the advised maximum **RangeA** distances would be 3.4 and 5.7 &#8491; respectively. However, these are not necessary for the needs of this tutorial.

{{< button pos="left" text="Previous Step" path="step5a/">}} {{< button pos="right" text="Next Step" path="step5c/">}}
