---
title: Step 3 - Set Up the Simulation
type: docs
weight: 1
---

### Create a Configuration

When importing the original CIF file a configuration was automatically created, so in this example we're ready to go.  It might be instructive to look at the nodes in the generator, specifcally the {{< node "Add" >}} node for the framework species. If you look at its control parameters you will see that the **BoxAction** parameter is `Set`, meaning that the configuration's box is set from that defined in the species.

### Set up Layers

Since our framework is one big periodic molecule there is no point in trying out Monte Carlo translation and rotation moves on it, so we'll evolve it with just molecular dynamics.

{{< action type="menu" text="Layer &#8680; Create &#8680; Evolution &#8680; MD (MD Only)" />}}

The {{< module "MD" >}} module in this layer runs every step and for a larger number of steps compared to the one in the standard MC/MD layer.

Finally, we need to calculate our correlation functions:

{{< action type="menu" text="Layer &#8680; Create &#8680; Correlations &#8680; RDF and Neutron S(Q)" />}}
{{< action type="tabs" >}}Go to the {{< gui-tab text="RDF / Neutron S(Q)" type="layer" >}} tab{{< /action >}}
{{< action type="mouse" >}}Click on the {{< module "SQ" >}} module to display its options{{< /action >}}
{{< action type="edit">}}Set the **QDelta** to `0.01`{{< /action >}}
{{< action type="edit">}}Set the **QBroadening** to `OmegaDependentGaussian` with a FWHM of `0.02`{{< /action >}}
{{< action type="mouse" >}}Click on the {{< module "NeutronSQ" >}} module to display its options{{< /action >}}
{{< action type="edit" text="For the **Reference** keyword in the **Reference Data** settings group, select the file `Empty-CuBTC.mint01` and set the format of the data to `mint`" />}}

We're changing to a finer binning in $Q$ so our Bragg features have better definition.

### Add Bragg Scattering

One thing missing from the standard layers is the calculation of Bragg scattering, so we must add a {{< module "Bragg" >}} module in to this layer.

{{< action type="mouse" >}}Show the module palette for the current layer by clicking the {{< gui-button icon="palette" text="Show Available Modules">}} button at the bottom of the module list on the left{{< /action >}}
{{< action type="mouse" >}}Drag a {{< module "Bragg" >}} module from the **Correlations** category up to the module list, placing it in-between the existing {{< module "GR" >}} and {{< module "SQ" >}} modules{{< /action >}}
{{< action type="mouse" >}}Select the new {{< module "Bragg" >}} module to show its options{{< /action >}}
{{< action type="edit" >}}Change the **QMax** to `5.0`{{< /action >}}

The {{< module "Bragg" >}} module is responsible only for calculating the reflection intensities from a target configuration - we must tell the {{< module "SQ" >}} module to take those intensities and incorporate them into the structure factors:

{{< action type="tabs" >}}Go to the {{< gui-tab text="RDF / Neutron S(Q)" type="layer" >}} tab{{< /action >}}
{{< action type="mouse" >}}Click on the {{< module "SQ" >}} module to display its options{{< /action >}}
{{< action type="edit" >}}Find the **Bragg Scattering** options group and set the **IncludeBragg** option to reference the only {{< module "Bragg" >}} module we have (called "Bragg"!){{< /action >}}

 * * *
{{< button pos="left" text="Previous Step" path="step2">}}
{{< button pos="right" text="Next Step" path="step4">}}
