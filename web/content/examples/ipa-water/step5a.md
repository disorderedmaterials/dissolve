---
title: Step 5a - g(r) and S(Q)
type: docs
weight: 1
---

We have neutron-weighted experimental data, so we need a layer to calculate g(r) and neutron-weighted S(Q) for our five experimental datasets:

{{< action type="menu" text="Layer &#8680; Create &#8680; Correlations &#8680; RDF and Neutron S(Q)" />}}

We need to set the instrumental broadening in the {{< gui-module "SQ" >}} module first, as this data was collected on SANDALS:

{{< action type="mouse">}}Click on the {{< gui-module "SQ" >}} module to select it{{< /action >}}
{{< action type="edit" text="Set the **QBroadening** to OmegaDependentGaussian with a FWHM of 0.02" />}}

Now let's set up the five {{< gui-module "NeutronSQ" >}} modules to calculate what we want:

{{< action type="mouse">}}Open the module palette for the layer by clicking the {{< gui-button icon="general_palette" text="Show Palette" >}} button at the bottom left of the tab{{< /action >}}
{{< step >}}Drag four additional {{< gui-module "NeutronSQ" >}} modules from the Correlation Functions category to the module list, placing them after the existing {{< gui-module "NeutronSQ" >}} module{{< /step >}}

Each of the five {{< gui-module "NeutronSQ" >}} will represent one of our experimental reference datasets. For example, the first one can be set up as follows:

{{< action type="mouse">}}Click on the first {{< gui-module "NeutronSQ" >}} module to select it{{< /action >}}
{{< action type="edit" text="Double-click on its name and change it to `9:1_D2O:IPA-d8`" />}}
{{< action type="edit" text="Set the **Exchangeable** option to the `HW` and `HO` atom types" />}}
{{< action type="edit" text="Set the **Isotopologues** to `D2O` and `IPA-D8`" />}}
{{< action type="groups" text="Open the **Reference Data** settings group" />}}
{{< step text="For the **Reference** keyword select the file '9:1_D2O:IPA-d8.mint01' and set the format of the data to `mint`" />}}

Once you've done this for all five, you're ready to proceed.

{{< button pos="left" text="Previous Step" path="step5/">}} {{< button pos="right" text="Next Step" path="step5b/">}}
