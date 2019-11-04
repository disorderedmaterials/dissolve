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
> Drag a second [`NeutronSQ`](/modules/neutronsq) over, placing it with the others
{: .step}

We'll now give the modules sensible names, and set the isotopologues, reference data files, and instrumental broadening as necessary. The latter, for SANDALS, is described by a _Q_-dependent Gaussian with a FWHM of 0.02 &#8491;.

### H<sub>2</sub>O

> Change the name of the first [`NeutronSQ`](/modules/neutronsq) module to "H2O"
{: .action .action_edit}
> Open the **Neutron Isotopes** settings group
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

> For generality, some of Dissolve's broadening functions refer to 'omega', which should be taken to mean the reciprocal space axis (in this case, 'Q').
{: .tip}

### D<sub>2</sub>O
> Change the name of the first [`NeutronSQ`](/modules/neutronsq) module to "D2O"
{: .action .action_edit}
> Open the **Neutron Isotopes** settings group
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

The HDO sample is a little different in respect of the isotopologue specification. In order to get the 50:50 mix we will basically add two isotopologues for the water species - one H<sub>2</sub>O and one D<sub>2</sub>O. Each will have the same relative weighting of 1.0. Importantly, we must also tell the module that the `HW` atom type is exchangeable - otherwise, the weighting of the intramolecular terms will be incorrect.

> As a general rule, any alcoholic or amine hydrogen is exchangeable.
{: .tip}

> Change the name of the first [`NeutronSQ`](/modules/neutronsq) module to "HDO"
{: .action .action_edit}
> Open the **Neutron Isotopes** settings group
{: .action .action_settings}
> Click the _Edit..._{: .text-green-100} button for the **Isotopologues** option
{: .step}
> Press the _Add_{: .text-green-100} button twice to add two isotopic specifications for water
{: .step}
> Change the isotopologue of one of them from `Natural` to `Deuterated`
{: .step}
> Click the button for the **Exchangeable** option
{: .step}
> Tick the `HW` atom type in the list
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

> Setting the **Exchangeable** atoms for a system is critical to getting the correct structure factors. For a mixture of isotopologues, even if there is only one exchangeable atom type. In the present example, not specifying that `HW` is exchangeable results in an effective simulation of explicit H<sub>2</sub>O and D<sub>2</sub>O molecules, whereas in reality their hydrogen atoms are (assumed to be) perfectly mixed.
{: .warn}

[Previous Step](step2.md){: .btn }   [Next Step](step4.md){: .btn .right}
