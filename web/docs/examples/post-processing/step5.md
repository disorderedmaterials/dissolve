---
title: Step 5 - Set Up Analysis
type: docs
weight: 1
---

We'll add a basic analysis layer and calculate the IPA-water centre-of-mass RDF as an example - feel free to add others into the layer as your curiosity dictates, but bear in mind that the test trajectory is quite small, so properties requiring a lot of statistics will not do well here.

{{< action type="menu" text="Layer &#8680; Create... &#8680; Empty" />}}
{{< step text="Double-click the layer tab and rename it to `Analyse`" />}}
{{< action type="mouse">}}Show the module palette for the current layer by clicking the {{< gui-button icon="general_palette" text="Show Available Modules">}} button at the bottom of the module list on the left{{< /action >}}
{{< action type="mouse">}}Drag a {{< gui-module "CalculateRDF" >}} module from the _Import_ section into the layer{{< /action >}}

In the {{< gui-module "CalculateRDF" >}} module itself:

{{< action type="edit">}}Set **SiteA** to the centre-of-mass site for IPA you created earlier{{< /action >}}
{{< action type="edit">}}Set **SiteB** to the centre-of-mass site for water{{< /action >}}

* * *
{{< button pos="left" text="Previous Step" path="step3/">}}
{{< button pos="right" text="Next Step" path="step4/">}}
