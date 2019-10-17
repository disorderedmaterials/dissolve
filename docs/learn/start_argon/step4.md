---
title: Step 4 - Set up the Simulation
grand_parent: Learn Dissolve
parent: Liquid Argon
nav_order: 4
---
# Liquid Argon

## 4) Set up the Simulation

It's time to tell Dissolve exactly what we want to do with this argon system, so we add some processing layers to the simulation. It makes sense to break up the simulation in to distinct parts (e.g. keeping evolution of the system separate from refinement or analysis) as layers can be turned on and off as the situation required. Each layer also has a frequency at which it will run, relative to the main simulation loop counter - a frequency of 1 means the layer will be executed every iteration. Modules within the layer also have an associated run frequency, but this is relative to the number of times the layer has been executed, rather than the main loop counter.

We'll split our simulation up in to three layers:

1. An 'evolution' layer which moves the contents of our configuration around and calculates the total energy of the system
2. A 'calculation' layer which periodically calculates the radial distribution function and structure factor
3. A 'refinement' layer which performs our potential refinement and drives the simulation towards agreement with the experimental data

### Evolving the System

We almost always want to move the contents of our configurations around, so first go to the _Layer_{: .text-green-100} menu and choose _Create...→Evolution...→Standard Atomic (MC/MD)_{: .text-green-100}. A new layer will be created containing all the modules that we need to do atomic Monte Carlo and a spot of molecular dynamics. Most modules need configuration targets to work on, and all available configurations (one in our case) will have been automatically set as a target in each of the new modules in the layer. Each module has its own defined frequency at which it will run, and can be temporarily disabled if necessary. There are usually a bunch of settings (accessed via the cog button) that control its action and, most importantly, the module may have entire control panels of other functionality (e.g. graphing output for various properties etc.). The latter cannot be viewed in layer tabs - instead, double-click on the module block to open an additional tab containing these controls, in addition to all of its settings.

All of the default settings for the modules within the evolution layer are sensible enough for our needs, so take a look around at what's there, but there's no need to change anything.

### Calculating g(r) and S(Q)

The experimental data we'll be fitting to is neutron scattering data, so we will need neutron-weighted structure factors. From the _Layer_{: .text-green-100} menu choose _Create...→Calculate...→RDF and Neutron S(Q)_{: .text-green-100}. This time we will need to set a few parameters in the `NeutronSQ` module, and we'll want to look at the plots of the structure factors, so double-click the module to open it up in a new tab. Now we have access to the options for the module, as well as any graphing or other facilities the module might provide.

On the left you'll see a categorised list of all available options - find and click the _Neutron Isotopes_{: .text-green-100} group, and you'll see an _Isotopologue_{: .text-green-100} option. This is where the isotopologues for all species in the configurations targetted by the `NeutronSQ` module must be set, so that the proper weighting of the calculated structure factors can be performed. Click the _Edit..._{: .text-green-100} button to show a drop-down list of all assigned isotopologues, which will currently be empty. Clicking the _Auto_{: .text-green-100} button will populate the list with any missing species (based on the current contents of the configuration). However, there are currently no isotopologues to assign to our

As it happens, the experimental data we'll be fitting is neutron scattering data, and was taken on Ar<sup>36</sup> since its coherent scattering cross-section (24.9 fm) is considerably higher than that of the naturally-occuring 'mix' (1.91 fm).

- Expand the entries for the 'Natural' isotopologue by clicking the [+] next to its name, or by clicking _Expand All_{: .text-green-100}.
- Each isotopologue contains an entry for every AtomType (not atom) used in the species for which single isotopes are specified.
- Change the entry from `Natural (bc = 1.909 fm)` to `36 (bc = 24.9)`.
- For sanity's sake, you may also want to change the name of the isotopologue from 'Natural' to something else (double-click on the name to do so).

Add Modules (2/5) Add Modules (3/5) It's nice to monitor the total energy of our system as it evolves, which is the function of the Energy Module:EnergyClick-drag one from the palette so it appears after the AtomShake, or double-click it in the palette. Since the AtomShake runs in every iteration of the layer, so should Energy, so leave the frequency as 1.The Energy Module also determines whether the calculated total energy is stable to within some threshold, and which other Modules can use in order to decide whether to perform their respective tasks. For instance, the EPSR Module (which we will add later) has an option which prevents it from trying to adjust the potential if the total energy of any of its target Configurations is not yet stable. This avoids potential problems that might arise from using a Configuration in an unequilibrated state. ]]> Add Modules (3/5) Add Modules (4/5) OK, to business! We ultimately want to calculate the total neutron-weighted structure factor for the system, but to do that we first need the total radial distribution function from the RDF Module:RDFAdd one after the Energy - note that RDF has a default frequency of 5, so this means we will calculate the g(r) only after the AtomShake and Energy have run five times each (as their frequencies are both 1). This prevents recalculating the g(r) too often, and slowing down the evolution of the system. ]]> Add Modules (4/5) Add Modules (5/5) From the data calculated by RDF we can calculate the structure factors by Fourier transform. Since we have neutron scattering data we will use the NeutronSQ Module:NeutronSQAdd one after the RDF.The NeutronSQ will need a minor amount of setting up, so let's get into the settings (click ) and get it ready for use.Neutron IsotopesWe need to inform NeutronSQ which isotopologue to use for each Species present over all of the targeted Configurations - in the present case, we just need to describe the scattering of the Argon species. Click the Auto button - this will add the 'Natural' isotopologue for each missing Species or, if this is not present, the first isotoplogue for that Species.

> **Useful Tip**
>
> You can do something amazing if you choose to try.
{: .tip }

[Previous Step](step3.md){: .btn }   [Next Step](step5.md){: .btn }
