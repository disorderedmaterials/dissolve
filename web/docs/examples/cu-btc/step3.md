---
title: Step 3 - Set Up the Simulation
type: docs
weight: 1
---

### Create a Configuration

Let's now set up a configuration for our simulation. We'll focus on just the Cu-BTC itself which means we essentially want to create a configuration with one copy of the Cu-BTC species in it, and which takes on its unit cell. Dissolve has a specific option for creating this kind of system:

{{< action type="menu" text="Configuration &#8680; Create..." />}}
{{< action type="mouse" >}}Choose the framework species and press {{< gui-button text="Next" icon="general_arrowright" >}}{{< /action >}}
{{< action type="mouse" >}}The configuration type is selected as _Framework + Mixture_ so press {{< gui-button text="Next" icon="general_arrowright" >}}{{< /action >}}
{{< action type="mouse" >}}The box style is now restricted to _Fixed Geometry, Fixed Size_ and the box lengths and angles can't be changed, so press {{< gui-button text="Next" icon="general_arrowright" >}}{{< /action >}}
{{< action type="mouse" >}}There is nothing to define on the species page, so click {{< gui-button text="Finish" icon="general_true" >}}{{< /action >}}
{{< action type="edit" text="Double-click on the configuration tab's title and change its name to `MOF`" />}}


If you click on the generator's {{< gui-node "Add" >}} node for the framework species and look at its control parameters you will see that the **BoxAction** parameter is `Set`, meaning that the configuration's box is set from that defined in the species.

### Set up Layers

Since our framework is one big periodic molecule there is no point in trying out Monte Carlo translation and rotation moves on it, so we'll evolve it with just molecular dynamics.

{{< action type="menu" text="Layer &#8680; Create &#8680; Evolution &#8680; MD (MD Only)" />}}

The {{< gui-module "MD" >}} module in this layer runs every step and for a larger number of steps compared to the one in the standard MC/MD layer.

Finally, we need to calculate our correlation functions:

{{< action type="menu" text="Layer &#8680; Create &#8680; Correlations &#8680; RDF and Neutron S(Q)" />}}
{{< action type="tabs" >}}Go to the {{< gui-tab text="RDF / Neutron S(Q)" type="layer" >}} tab{{< /action >}}
{{< action type="mouse" >}}Click on the {{< gui-module "SQ" >}} module to display its options{{< /action >}}
{{< action type="edit">}}Set the **QDelta** to `0.01`{{< /action >}}
{{< action type="edit">}}Set the **QBroadening** to `OmegaDependentGaussian` with a FWHM of `0.02`{{< /action >}}
{{< action type="mouse" >}}Click on the {{< gui-module "NeutronSQ" >}} module to display its options{{< /action >}}
{{< action type="groups" text="Open the **Reference Data** settings group" />}}
{{< action type="edit" text="For the **Reference** keyword select the file `Empty-CuBTC.mint01` and set the format of the data to `mint`" />}}

We're changing to a finer binning in $Q$ so our Bragg features have better definition.

### Add Bragg Scattering

One thing missing from the standard layers is the calculation of Bragg scattering, so we must add a {{< gui-module "Bragg" >}} module in to this layer.

{{< action type="mouse" >}}Show the module palette for the current layer by clicking the {{< gui-button icon="general_palette" text="Show Available Modules">}} button at the bottom of the module list on the left{{< /action >}}
{{< action type="mouse" >}}Drag a {{< gui-module "Bragg" >}} module from the **Correlations** category up to the module list, placing it in-between the existing {{< gui-module "GR" >}} and {{< gui-module "SQ" >}} modules{{< /action >}}
{{< action type="mouse" >}}Select the new {{< gui-module "Bragg" >}} module to show its options{{< /action >}}
{{< action type="edit" >}}Change the **QMax** to `5.0`{{< /action >}}

The {{< gui-module "Bragg" >}} module is responsible only for calculating the reflection intensities from a target configuration - we must tell the {{< gui-module "SQ" >}} module to take those intensities and incorporate them into the structure factors:

{{< action type="tabs" >}}Go to the {{< gui-tab text="RDF / Neutron S(Q)" type="layer" >}} tab{{< /action >}}
{{< action type="mouse" >}}Click on the {{< gui-module "SQ" >}} module to display its options{{< /action >}}
{{< action type="groups" >}}Open the **Bragg Scattering** options group{{< /action >}}
{{< step >}}Set the **IncludeBragg** option to reference the only {{< gui-module "Bragg" >}} module we have (called "Bragg"!){{< /step >}}

 * * *
{{< button pos="left" text="Previous Step" path="step2">}}
{{< button pos="right" text="Next Step" path="step4">}}
