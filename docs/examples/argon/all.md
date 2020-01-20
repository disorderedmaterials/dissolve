---
title: Liquid Argon
parent: Examples
has_children: true
---
# Liquid Argon

20 minute read
{: .label .label-blue }

## Summary

Liquid argon isn't a particularly exciting system, but it has been measured experimentally by neutron scattering, and it serves as an excellent example to introduce the basic workflows of Dissolve. This example will show you how to set up a complete working simulation from scratch, with the purpose to simulate liquid argon at 85 K and compare the simulation to reference neutron data from ["Structure Factor and Radial Distribution Function for Liquid Argon at 85 °K"](https://journals.aps.org/pra/abstract/10.1103/PhysRevA.7.2130), J. L. Yarnell, M. J. Katz, R. G. Wenzel, and S. H. Koenig, _Phys. Rev. A_ **7**, 2130.

## Preparation

Download the example data file archive `Dissolve-X.Y.Z-ExampleData.zip` or `Dissolve-X.Y.Z-ExampleData.tar.gz` (where `X.Y.Z` will be the current version) from the [continuous build](https://github.com/trisyoungs/dissolve/releases/tag/continuous) in the releases section on GitHub if you haven't done so already.

> Open the Dissolve GUI
{: .action .action_dissolve}
> Choose **Empty Simulation** from the _Create_ section
{: .step}
> Save As...
{: .action .action_menu}
> Save your own input file under a sensible name in the `argon` directory of the example data
{: .step}

Alternatively, download the data files listed below and save them to your location of choice.

- Neutron data file: [yarnell.sq](https://raw.githubusercontent.com/trisyoungs/dissolve/develop/examples/argon/data/yarnell.sq)

[Let's begin!](step1.md){: .btn}


## 1. Creating a Species

We need a suitable species to represent the argon atom in our simulation.

> Species &#8680; Create...→Atomic
{: .action .action_menu}
> Select argon (Ar) from the periodic table
{: .step}

You will now see that a new tab called 'Ar' has appeared containing the new species, showing a single argon atom located at (0,0,0). If you go to the _Contents & Forcefield_{: .text-green-100} section you can the details of the species contents (which is not a lot!). Our argon atom currently has no atom type assigned to it, but we will remedy this in the next step. All atoms **must** be assigned an atom type before the simulation can proceed.

> An atom type contains descriptive interaction parameters for a particular type of atom, and optionally an associated atomic charge. They describe the core interatomic interactions in your system, and define the pair potentials between atoms.
{: .tip}

Note also the presence of the _Isotopologues_{: .text-green-100} section which allows us to define isotopes for specific atom types, and which we'll return to later.

> Isotopologue definitions depend on atom types, so all atom types should be defined before setting up isotopologues.
{: .tip }

[Back to Overview](index.md){: .btn }   [Next Step](step2.md){: .btn .right}

## 2. Set up a Forcefield

To simulate any system we need to supply suitable parameters to describe the interactions between atoms, both internally within the same species and externally between atoms of the same (and different) species. The _Forcefield_{: .text-green-100} tab is always available, and summarises the atom types used over all defined species and the resulting pair potentials, as well as master intramolecular definitions (which we aren't using in this example). You can also set other important options, such as the range of the generated pair potentials and the truncation schemes.

We'll now describe the interactions for your argon species by taking the terms from one of Dissolve's [built-in forcefields](../../setup/forcefields.md).

> **Species (Argon)** tab
{: .action .action_tabs}
> Species &#8680; Add Forcefield Terms
{: .action .action_menu}
> From the available forcefields choose `OPLSAA2005/NobleGases` and click **Next**
{: .step}
> For the atom types we want to **Assign all from Forcefield**, and we have no intramolecular terms so can ignore the section below.
{: .step}
> There will be no conflicts between atom types or intramolecular terms, so click **Next** through those sections and then **Finish** the wizard.
{: .step}

> For more complicated (molecular) species you may want to change how (or which) _intra_-molecular terms are generated. You also may have to deal with potential naming conflicts with existing terms, which the _Add Forcefield Terms_ wizard will help you resolve.
{: .tip}

If you now return to the _Forcefield_{: .text-green-100} tab you will see in the _Atom Types_{: .text-green-100} section that we now have an atom type for our argon atom that contains the necessary interaction parameters. The short range type (_SR Form_{: .text-green-100} in the table) specifies the functional form of the van der Waals interaction parameters for each atom type, which in our case is `LJGeometric` indicating that the parameters reflect a standard Lennard-Jones potential utilising geometric combination rules.

> Atom types are strictly associated with a chemical element, and can only be applied to atoms of the same element.
{: .warn}

By default, Dissolve will generate all the necessary pair potentials for the current set of atom types automatically - these are listed in the _Pair Potentials_{: .text-green-100} section where you can select individual potentials and visualise them in the associated plot. Before we move on, we'll reduce the pair potential range for the simulation.

> **Forcefield** tab, **Pair Potentials** section
{: .action .action_tabs}
> Reduce the pair potential **Range** from 15 to 10
{: .step}

[Previous Step](step1.md){: .btn }   [Next Step](step3.md){: .btn .right}

## 3. Add a Configuration

A configuration represents the target system using one or more of the available species. Our next step is to create a configuration that is consistent with the experimentally-measured system investigated in the original paper.

Configurations in Dissolve are created from a sequence of instructions that define box shape and size, the populations of species to add etc. This permits Dissolve to automatically regenerate configurations when it needs to, and is particularly useful when screening structures with variable parameters (density, composition, pore size etc.).

Dissolve provides several standard configuration generators for common-use cases, one of which we will use now.

> Configuration &#8680; Create... &#8680; Simple random mix
{: .action .action_menu}
> A random mix can contain any number of species - we only have one, so select the argon species
{: .step}

You will now see that the _Generator_{: .text-green-100} section has been populated with several steps, and a new configuration of atomic coordinates has been created based on these steps.  We'll go through these one-by-one in the order they appear. To see the settings for any step, click the cog icon in its bottom-right corner.

| Node | Purpose |
|------|---------|
| [`Parameters`](../../userguide/nodes/parameters) | Defines one or more numerical parameters that can be referenced by other nodes in the generator. We have only one (floating point) value called `rho` which defines the density of the configuration. Note that `rho` is just a number and has no associated units - these are specified in the [`AddSpecies`](../../userguide/nodes/addspecies) node. |
| [`Box`](../../userguide/nodes/box)   | Defines side lengths and angles for the periodic box. Note that _relative_ lengths can be given, and the box expanded automatically (when adding species, for instance) to achieve some specific density. |
| [`AddSpecies`](../../userguide/nodes/addspecies) | Adds a number of copies of the specified species to the configuration. The option _ScaleBox_{: .text-green-100} controls whether the box volume should be scaled to give the density specified once the species has been added, and is most useful when supplying relative box lengths. If turned off, the box would remain at its current size (defaulting to 1.0 &#8491;<sup>3</sup>) but the full population of molecules will still be added. |

In the `AddSpecies` node note how we have reference the `rho` parameter for the density. Many numerical values for nodes 

> Many numerical options can be given in the form of equations referencing variables such as those set in a `Parameters` node. A green tick indicates if the equation is valid.
{: .tip}

Let's change the settings of our configuration's generator to give us a suitable system.

> Open the settings for the `Parameters` node
{: .action .action_settings}
> Change the value of the "rho" variable to 0.0213 (which is our target density in units of atoms/A3)
{: .step}
> Open the settings for the `AddSpecies` node
{: .action .action_settings}
> Check that the units for the **Density** are set to `atoms/A3`
{: .step}
> Increase the **Population** to 1000
{: .step}

Finally, we need to set the correct temperature for the configuration.

> Set the **Temperature** to 85 K to match that of the experimental measurement
{: .action .action_edit}

Because we've changed some settings, we need to hit the _Regenerate_{: .text-green-100} button to recreate our initial configuration.

> You can regenerate a configuration at any time, but bear in mind that other quantities calculated by modules may also need to be cleared. Remove all of this data to begin a 'clean' run with the _Clear Module Data_{: .text-green-100} option in the  _Simulation_{: .text-green-100} menu.
{: .tip}

[Previous Step](step2.md){: .btn }   [Next Step](step4.md){: .btn .right}

## 4a) Evolving the System

We almost always want to move the contents of our configurations around, and this is the job of the evolution layer.

> Layer &#8680; Create... &#8680; Evolution... &#8680; Standard Atomic (MC/MD)
{: .action .action_menu}

Our new layer contains the following modules:

| Module | Purpose |
|--------|---------|
| [`AtomShake`](../../userguide/modules/atomshake) | Performs standard Monte Carlo moves on individual atoms |
| [`MD`](../../userguide/modules/md) | Performs a number of molecular dynamics steps, evolving the system according to Newton's equations of motion |
| [`Energy`](../../userguide/modules/energy) | Calculates the total energy, including contributions from intramolecular terms if present |

Selecting any module in the list on the left will show its full options in the panel on the right - select the [`AtomShake`](../../userguide/modules/atomshake) to see the target acceptance ratio, step size, etc.

Most modules need configuration targets to work on, and all available configurations (one in our case) will have been automatically set as a target in each of the new modules in the layer. As mentioned, each module has its own defined frequency at which it will run within the layer - [`AtomShake`](../../userguide/modules/atomshake) and [`Energy`](../../userguide/modules/energy) modules will run every time the layer is run (frequency = 1) while the [`MD`](../../userguide/modules/md) module will only run every fifth step. Each module can be individually enabled / disabled using the slider, with a green tick indicating that the module is active.

Finally, modules may have entire control panels of other functionality, most commonly graphing output for various properties etc. This cannot be viewed within a layer tab - instead, double-click on the module block to open an additional tab containing these controls, in addition to all of its settings.

All of the default settings for the modules within the evolution layer are sensible enough for our needs, so take a look around at what's there if you want to, but there's no need to change anything.

[Previous Step](step4.md){: .btn }   [Next Step](step4b.md){: .btn .right}

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

## 4) Set up the Simulation

It's time to tell Dissolve exactly what we want to do with this argon system. All of Dissolve's "useful" work is achieved by sequences of [modules](../../userguide/modules) within layers, and it makes sense to break up simulations into distinct parts (e.g. keeping evolution of the system separate from refinement or analysis) as layers can be turned on and off as the situation requires. Each layer also has a frequency at which it will run, relative to the main simulation loop counter - a frequency of 1 means the layer will be executed every iteration. Modules within the layer also have an associated run frequency, but this is relative to the number of times the layer has been executed, rather than the main loop counter.

We'll split our simulation up into two layers:

1. An 'evolution' layer which moves the contents of our configuration around and calculates the total energy of the system
2. A 'calculation' layer which periodically calculates the radial distribution function and neutron-weighted structure factor

> You can have as many layers in a simulation as you need - whatever makes the most sense in what you're trying to do.
{: .tip}

[Previous Step](step3.md){: .btn }   [Next Step](step4a.md){: .btn .right}

## 5) Equilibrate the System

With two of our processing layers set up we can start to run the simulation and monitor how the system is evolving. There are several ways to run the simulation within the Dissolve GUI.

> Click the _Run_{: .text-green-100} button on the left-hand side of the main window
{: .action .action_mouse}
> Simulation &#8680; Run
{: .action .action_menu}
> Ctrl-R
{: .action .action_key}

Do one of those to set the simulation running. Note the counter in the top-left of the main window which tracks the current iteration.

While the simulation is running you cannot edit any input values, keywords etc., but you can investigate the simulation's progress and output as it happens. For example, you could go to your _Standard Atomic (MC/MD)_ evolution layer and double-click the [`Energy`](../../userguide/modules/energy) module to see a plot of the total energy of the configuration.

If you have the [`NeutronSQ`](../../userguide/modules/neutronsq) module tab still open (if you don't, go back to your _RDF and Neutron S(Q)_ layer and double-click on the [`NeutronSQ`](../../userguide/modules/neutronsq) module) you can see how the calculated structure factor and total G(r) compare to the experimental data we loaded in. After the simulation has been running for a little while (perhaps 100 iterations), you'll see that the simulation and experiment actually compare quite favourably, with the G(r) and F(Q) looking something like this:

![](equilibrated-gr.png){: .img-centre}
*Equilibrated total G(r) for liquid argon*
![](equilibrated-fq.png){: .img-centre}
*Equilibrated total F(Q) for liquid argon*

We'll now stop the simulation.

> Click the _Pause_{: .text-green-100} button on the left-hand side of the main window
{: .action .action_mouse}
> Simulation &#8680; Pause
{: .action .action_menu}
> Esc
{: .action .action_key}

Keep in mind that the simulation will not actually stop until the current iteration is completed - most parts of the GUI will remain grayed out until then.

[Previous Step](step4b.md){: .btn }   [Next Step](step6.md){: .btn .right}

## 6) Next Steps

Now that our simulation is set up and running nicely, we will consider the quality of our calculations against the reference experimental data. The reference potential which we took from an off-the-shelf forcefield actually does an excellent job of reproducing the data, so there is very little else to do in terms of refining the potential in order to get better agreement. We also have limited options for analysing further the structure of the system - for a single-component atomic fluid the radial distribution function pretty much tells you everything you need to know.

You're now prepared for more useful / interesting / exciting (delete as appropriate) examples - a good next step would be to try the [liquid water example](../water).

[Previous Step](step5.md){: .btn }      [Back to Index](index.md){: .btn .right}
