---
title: Step 6 - Set up Refinement
parent: Comparing Benzene Structure
grand_parent: Examples
---
# Liquid Water

## 6. Set up Refinement

Time to refine. We'll need two processing layers - one to calculate the g(r) and S(Q) from the simulation, and one to refine it against experimental data.

As noted in the introduction we have neutron-weighted experimental data, so we need a layer to calculate g(r) and neutron-weighted S(Q) for our three experimental datasets:

> Layer &#8680; Create... &#8680; Calculate... &#8680; RDF and Neutron S(Q)
{: .action .action_menu}
> Show the module palette for the layer by clicking the _Show Palette_{: .text-green-100} button at the top left of the tab.
{: .action .action_mouse}
> Drag two additional [`NeutronSQ`](../../userguide/modules/neutronsq) modules from the `Correlation Functions` in the _Module Palette_{: .text-green-100} on the left, placing them after the existing [`RDF`](../../userguide/modules/rdf) module
{: .step}

Let's set up the three [`NeutronSQ`](../../userguide/modules/neutronsq) modules to calculate what we want:

### C<sub>6</sub>H<sub>6</sub>

> Click on the first the [`NeutronSQ`](../../userguide/modules/neutronsq) module to display its options
{: .action .action_mouse}
> Change its name to `C6H6`
{: .action .action_edit}
> Open the **Calculation** settings group
{: .action .action_groups}
> Set the **QBroadening** to `OmegaDependentGaussian` with a FWHM of 0.02
{: .step}
> Open the **Neutron Isotopes** settings group
{: .action .action_groups}
> Click the _Edit..._{: .text-green-100} button for the **Isotopologue** option
{: .step}
> Press the _Auto_{: .text-green-100} button to populate the list with the default isotopic selection for each species
{: .step}
> Open the **Reference Data** settings group
{: .action .action_groups}
> For the **Reference** keyword select the file `C6H6.mint01` and set the format of the data to `mint` instead of `xy`
{: .step}

### C<sub>6</sub>D<sub>6</sub>

> Click on the second the [`NeutronSQ`](../../userguide/modules/neutronsq) module to display its options
{: .action .action_mouse}
> Change its name to `C6D6`
{: .action .action_edit}
> Open the **Calculation** settings group
{: .action .action_groups}
> Set the **QBroadening** to `OmegaDependentGaussian` with a FWHM of 0.02
{: .step}
> Open the **Neutron Isotopes** settings group
{: .action .action_groups}
> Click the _Edit..._{: .text-green-100} button for the **Isotopologue** option
{: .step}
> Press the _Auto_{: .text-green-100} button to add the natural isotopologue for each species present
{: .step}
> Change the isotopologue from `Natural` to `Deuterated`
{: .step}
> Open the **Reference Data** settings group
{: .action .action_groups}
> For the **Reference** keyword select the file `C6D6.mint01` and set the format of the data to `mint`
{: .step}

### 50:50 Mix

> Click on the third the [`NeutronSQ`](../../userguide/modules/neutronsq) module to display its options
{: .action .action_mouse}
> Change its name to `5050`
{: .action .action_edit}
> Open the **Calculation** settings group
{: .action .action_settings}
> Set the **QBroadening** to `OmegaDependentGaussian` with a FWHM of 0.02
{: .step}
> Open the **Neutron Isotopes** settings group
{: .action .action_settings}
> Click the _Edit..._{: .text-green-100} button for the **Isotopologue** option
{: .step}
> Press the _Auto_{: .text-green-100} button to add the natural isotopologue for each species present
{: .step}
> Select the entry for the benzene species, or its isotopologue, and click the _Add_{: .text-green-100} button to insert the next "unused" isotopologue (i.e. the deuterated analogue).
{: .step}
> Open the **Reference Data** settings group
{: .action .action_settings}
> For the **Reference** keyword select the file `5050.mint01` and set the format of the data to `mint`
{: .step}

All that remains is to add our EPSR refinement layer:

> Layer &#8680; Create... &#8680; Refinement... &#8680; Standard EPSR
{: .action .action_menu}
> Select the [`EPSR`](../../userguide/modules/epsr) module to display its options
{: .step}
> Change the **EReq** value to 10.0
{: .action .action_edit}

[Previous Step](step5.md){: .btn }   [Next Step](step7.md){: .btn .right}
