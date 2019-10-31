---
title: Step 3 - Set up Basic Processing
parent: Liquid Water
grand_parent: Examples
---
# Liquid Water

## 3. Set up Basic Processing

We'll now create two processing layers to handle the evolution of the configuration and the calculation of radial distribution functions and structure factors.

> Layer &#8680; Create... &#8680; Evolution... &#8680; Standard Molecular (MC/MD)
{: .action .action_menu}

The default settings will give us what we need, which is molecular Monte Carlo every iteration of the main loop, and a short Molecular Dynamics run every five iterations. The latter is particularly important as this will give intramolecular variety to the molecules - if it were not present, every water molecule would have exactly the same geometry, and this is most definitely not what we want.

Now we can add a calculation layer for the g(r) and structure factors.

> Layer &#8680; Create... &#8680; Evolution... &#8680; Standard Molecular (MC/MD)
{: .action .action_menu}

Since a [`NeutronSQ`](/modules/neutronsq) module calculates the partial and total structure factors for a specific isotopic composition, we will need to add two more since we have three reference datasets.

> Drag a [`NeutronSQ`](/modules/neutronsq) module from the _Palette_{: .text-green-100} on the left, placing it after the existing [`NeutronSQ`](/modules/neutronsq) module
{: .action .action_mouse}
> Drag a second [`NeutronSQ`](/modules/neutronsq) over, again placing it at the bottom of the layer
{: .step}

We'll now give the modules sensible names, and set the isotopologues, reference data files, and instrumental broadening as necessary. The latter, for SANDALS, is described by a Q-dependent Gaussian with a FWHM of 0.02 Angstroms.

### H<sub>2</sub>O

> Change the name of the first [`NeutronSQ`](/modules/neutronsq) module to "H2O"
{: .action .action_edit}
> Expand the settings for the module with the ![Toggle settings button](../icons/general_settings.png) and open the **Neutron Isotopes** settings group
{: .action .action_settings}
> Click the _Edit..._{: .text-green-100} button for the **Isotopologues** option
{: .step}
> Press the _Auto_{: .text-green-100} button to populate the list with the default isotopic selection for each species
{: .step}
> Open the **Reference Data** settings group
{: .action .action_settings}
> Click the _Set..._{: .text-green-100} button for the **Reference** option
{: .step}
> Select the file `SLS18498-H2O.mdcs01` - the format is `mint`
{: .step}
> Open the **Calculation** settings group
{: .action .action_settings}
> Set the **QBroadening** to `OmegaDependentGaussian` with a FWHM of 0.02
{: .step}

### D<sub>2</sub>O
> Change the name of the first [`NeutronSQ`](/modules/neutronsq) module to "D2O"
{: .action .action_edit}
> Expand the settings for the module with the ![Toggle settings button](../icons/general_settings.png) and open the **Neutron Isotopes** settings group
{: .action .action_settings}
> Click the _Edit..._{: .text-green-100} button for the **Isotopologues** option
{: .step}
> Press the _Auto_{: .text-green-100} button to populate the list with the default isotopic selection for each species
{: .step}
> Change the isotopologue from `Natural` to `Deuterated`
{: .step}
> Open the **Reference Data** settings group
{: .action .action_settings}
> Click the _Set..._{: .text-green-100} button for the **Reference** option
{: .step}
> Select the file `SLS18502-D2O.mdcs01` - the format is `mint`
{: .step}
> Open the **Calculation** settings group
{: .action .action_settings}
> Set the **QBroadening** to `OmegaDependentGaussian` with a FWHM of 0.02
{: .step}

### HDO

The HDO sample is a little different in respect of the isotopologue specification. In order to get the 50:50 mix we will basically add two isotopologues for the water species - one H<sub>2</sub>O and one D<sub>2</sub>O. Each will have the same relative weighting of 1.0.

> Change the name of the first [`NeutronSQ`](/modules/neutronsq) module to "HDO"
{: .action .action_edit}
> Expand the settings for the module with the ![Toggle settings button](../icons/general_settings.png) and open the **Neutron Isotopes** settings group
{: .action .action_settings}
> Click the _Edit..._{: .text-green-100} button for the **Isotopologues** option
{: .step}
> Press the _Add_{: .text-green-100} button twice to add two isotopic specifications for water
{: .step}
> Change the isotopologue of one of them from `Natural` to `Deuterated`
{: .step}
> Open the **Reference Data** settings group
{: .action .action_settings}
> Click the _Set..._{: .text-green-100} button for the **Reference** option
{: .step}
> Select the file `SLS18502-D2O.mdcs01` - the format is `mint`
{: .step}
> Open the **Calculation** settings group
{: .action .action_settings}
> Set the **QBroadening** to `OmegaDependentGaussian` with a FWHM of 0.02
{: .step}

[Previous Step](step2.md){: .btn }   [Next Step](step4.md){: .btn .right}
