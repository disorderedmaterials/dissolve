---
title: Step 4b - Calculate g(r) and S(Q)
grand_parent: Examples
parent: Liquid Argon
nav_order: 6
---
# Liquid Argon

## 4b) Calculate g(r) and S(Q)

The experimental data we'll be fitting to is neutron scattering data, so we will need to calculate neutron-weighted structure factors.

> Layer &#8680; Create... &#8680; Calculate... &#8680; RDF and Neutron S(Q)
{: .action .action_menu}

The new layer contains the following modules:

| Module | Purpose |
|--------|---------|
| [`RDF`](../../userguide/modules/rdf) | Calculates partial g(r) between every pair of atom types, and sums them into the total G(r) |
| [`NeutronSQ`](../../userguide/modules/neutronsq) | Fourier transforms the g(r) calculated by an [`RDF`](../../userguide/modules/rdf) module to generate S(Q) and the neutron-weighted total structure factor F(Q) |

This time we will need to set a few parameters in the [`NeutronSQ`](../../userguide/modules/neutronsq) module, in particular informing it of the isotopic composition of our system and loading in reference data.

> A [`NeutronSQ`](../../userguide/modules/neutronsq) module calculates S(Q) and F(Q) for a single isotopic composition. Where there are multiple isotopic samples, you need to add a [`NeutronSQ`](../../userguide/modules/neutronsq) for each distinct isotopic composition.
{: .warn }

### Set up Isotopes

The [`NeutronSQ`](../../userguide/modules/neutronsq) module will use isotopic natural abundances to calculate the neutron weights for all species unless we tell it otherwise. We'll first define the correct isotopologue for our argon species, and then tell [`NeutronSQ`](../../userguide/modules/neutronsq) to use it. The experimental measurement was made on Ar<sup>36</sup> since its coherent scattering cross-section (24.9 fm) is considerably higher than that of the naturally-occuring mix (1.91 fm).

> **Species (Argon)** tab, **Isotopologues** section
{: .action .action_tabs}
> Click _Add_{: .text-green-100}
{: .step}
> Expand the entries for the 'NewIsotopologue' by clicking the [+] next to its name, or by clicking _Expand All_{: .text-green-100}
{: .step}
> Change the entry for the Ar atom type from `Natural (bc = 1.909 fm)` to `36 (bc = 24.9)`
{: .step}
>For sanity's sake, you may also want to change the name of the isotopologue to something like 'Ar36' (double-click on the name to do so)
{: .step}

Now, we'll open up our [`NeutronSQ`](../../userguide/modules/neutronsq) module in a tab of its own.

> Double-click the [`NeutronSQ`](../../userguide/modules/neutronsq) module on its icon or name
{: .action .action_mouse}

In this tab we have access to all the options for the module, as well as the plots for the calculated structure factors. On the left you'll see a categorised list of all available options where we will specify our isotpologue.

> **NeutronSQ** tab
{: .action .action_tabs}
> Open the **Neutron Isotopes** settings group
{: .action .action_groups}
> Click the _Edit..._{: .text-green-100} button for the **Isotopologue** option
{: .step}
> Press the _Auto_{: .text-green-100} button to populate the list with the default isotopic selection for each species
{: .step}
> Change the isotopologue for the argon species from `Natural` to `Ar36`
{: .step}

> The 'Natural' isotopologue for each species is defined internally by Dissolve, and is always available. It does not appear in the list of defined isotopologues on the species tab.
{: .tip }

### Import Reference Data

The [`NeutronSQ`](../../userguide/modules/neutronsq) module itself looks after any related experimental reference data that we might wish to compare our calculated data to, and which we'll now set up.

> **NeutronSQ** tab
{: .action .action_tabs}
> Open the **Reference Data** settings group
{: .action .action_groups}
> For the **Reference** keyword select the file `yarnell.sq`, and set the format of the data to `xy`
{: .step}
> Open the options for the file import 
{: .action .action_settings}
> Open the **Manipulations** group
{: .action .action_groups}
> We need to subtract the average level from the experimental data as it oscillates around 1.0, so set the **RemoveAverage** value to 9.0. This will instruct Dissolve to work out the average value of the data from x = 9.0, and remove it from the data.
{: .step}

The data, along with its Fourier transform, are now plotted in the graphs to the right, and you'll see that the data file name now appears in the button for the **Reference** keyword. The data are normalised to the average squared value of the atomic scattering, so we will instruct Dissolve to remove this normalisation.

> **NeutronSQ** tab
{: .action .action_tabs}
> Open the **Reference Data** settings group
{: .action .action_groups}
> Select `AverageSquared` for the **ReferenceNormalisation**
{: .step}

[Previous Step](step4a.md){: .btn }   [Next Step](step5.md){: .btn .right}
