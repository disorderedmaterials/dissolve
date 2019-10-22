---
title: Step 4b - Calculate g(r) and S(Q)
grand_parent: Learn Dissolve
parent: Liquid Argon
nav_order: 6
---
# Liquid Argon

## 4b) Calculate g(r) and S(Q)

The experimental data we'll be fitting to is neutron scattering data, so we will need neutron-weighted structure factors. From the _Layer_{: .text-green-100} menu choose _Create...→Calculate...→RDF and Neutron S(Q)_{: .text-green-100}. The new layer contains the following modules:

| Module | Purpose |
|--------|---------|
| [`RDF`](/modules/rdf) | Calculates partial g(r) between every pair of atom types, and sums them into the total G(r) |
| [`NeutronSQ`](/modules/neutronsq) | Fourier transforms the g(r) calculated by an [`RDF`](/modules/rdf) module to generate S(Q) and the neutron-weighted total structure factor F(Q) |

This time we will need to set a few parameters in the [`NeutronSQ`](/modules/neutronsq) module, in particular informing it of the isotopic composition of our system and loading in reference data.

> A [`NeutronSQ`](/modules/neutronsq) module calculates S(Q) and F(Q) for a single isotopic composition. Where there are multiple isotopic samples, you need to add a [`NeutronSQ`](/modules/neutronsq) for each distinct isotopic composition.
{: .warn }

### Set up Isotopes

Double-click the [`NeutronSQ`](/modules/neutronsq) module to open it up in a new tab. Now we have access to the options for the module, as well as any graphing or other facilities the module might provide. On the left you'll see a categorised list of all available options - find and click the _Neutron Isotopes_{: .text-green-100} group, and you'll see an Isotopologue{: .text-green-100} option. This is where the isotopologues for all species in the configurations targetted by the [`NeutronSQ`](/modules/neutronsq) module must be set, so that the proper weighting of the calculated structure factors can be performed. Click the _Edit..._{: .text-green-100} button to show a drop-down list of all assigned isotopologues, which will currently be empty. Clicking the _Auto_{: .text-green-100} button will populate the list with any missing species (based on the current contents of the configuration) and assign them the natural isotopologue.

> The 'Natural' isotopologue for each species is defined internally by Dissolve, and is always available.
{: .tip }

The experimental measurement was made on Ar<sup>36</sup> since its coherent scattering cross-section (24.9 fm) is considerably higher than that of the naturally-occuring 'mix' (1.91 fm). So, we'll need to create a new isotopologue definition in the species to reference in the [`NeutronSQ`](/modules/neutronsq) module:

- Return to the _Isotopologues_{: .text-green-100} section of your argon species.
- Click _Add_{: .text-green-100}.
- Expand the entries for the 'NewIsotopologue' by clicking the [+] next to its name, or by clicking _Expand All_{: .text-green-100}.
- Change the entry for the Ar atom type from `Natural (bc = 1.909 fm)` to `36 (bc = 24.9)`.
- For sanity's sake, you may also want to change the name of the isotopologue to something like 'Ar36' (double-click on the name to do so).

> Isotopic mixtures for the same atom type are created by mixing isotopologue definitions rather than specifying the isotope ratio within the isotopologue itself.
{: .warn }

Now you can return to the tab for your [`NeutronSQ`](/modules/neutronsq) module and _Edit..._{: .text-green-100} the **Isotopologue** keyword. You'll now find that the options for the isotopologues for your Ar species include the one you just created, so select it.

### Import Reference Data

The [`NeutronSQ`](/modules/neutronsq) module itself looks after any related experimental reference data. Go to the _Reference Data_{: .text-green-100} group and set up our reference argon data:

- Click the "Set..." button for the **Reference** keyword to show the file selection dropdown.
- Open the file `yarnell.sq` from the `examples/argon` data directory (the format of the file is simple `xy` data).
- We need to subtract the average level from the experimental data as it oscillates around 1.0, so go to _Manipulations_{: .text-green-100} in the options and set the **RemoveAverage** value to 9.0.

The data, along with its Fourier transform, are now plotted in the graphs to the right, and you'll see that the data file name now appears in the button for the **Reference** keyword. The data are normalised to the average squared value of the atomic scattering, so choose `AverageSquared` for the **ReferenceNormalisation** to instruct Dissolve to remove this normalisation.

[Previous Step](step4a.md){: .btn }   [Next Step](step4c.md){: .btn .right}
