---
title: Step 3 - Add a Configuration
grand_parent: Examples
parent: Liquid Argon
nav_order: 3
---
# Liquid Argon

## 3. Add a Configuration

A configuration represents the target system using one or more of the available species. Our next step is to create a configuration that is consistent with the experimentally-measured system investigated in the original paper.

Configurations in Dissolve are created from a sequence of instructions that define box shape and size, the populations of species to add etc. This permits Dissolve to automatically regenerate configurations when it needs to, and is particularly useful when screening structures with variable parameters (density, composition, pore size etc.).

Dissolve provides several standard generators for common-use cases, one of which we will use now.

- From the _Configuration_{: .text-green-100} menu choose _Create...→Simple random mix_{: .text-green-100}).
- A random mix can contain any number of species. We only have one, so select the argon species from the dialog that appears.

You will now see that the _Generator_{: .text-green-100} section has been populated with several steps, and a new configuration of atomic coordinates has been created based on these steps.  We'll go through these one-by-one in the order they appear. To see the settings for any step, click the cog icon in its bottom-right corner.

| Node | Purpose |
|------|---------|
|Parameters | Defines one or more numerical parameters that can be referenced by other nodes in the generator. We have only one (floating point) value called `rho` which defines the density of the configuration. |
|Box   | Defines side lengths and angles for the periodic box. Note that _relative_ lengths can be given, and the box expanded automatically (when adding species, for instance) to achieve some specific density. |
|AddSpecies | Adds a number of copies of the specified species to the configuration. The option _ScaleBox_{: .text-green-100} controls whether the box volume should be scaled to give the density specified for the species, and is used when supplying relative box lengths. If turned off, the box would remain at its current size (defaulting to 1.0 cubic Angstrom) but the full population of molecules will still be added. |

In the `AddSpecies` node note how we have used the `rho` parameter for the density.

> Many numerical options can be given in the form of equations referencing variables such as those set in a `Parameters` node. A green tick indicates if the equation is valid.
{: .tip}

Let's change the settings of our configuration and its generator to give us a suitable system.

- Change the value of `rho` in the `Parameters` node to 0.0213, and ensure that the units in the `AddSpecies` node are set to `atoms A-3`.
- Increase the _Population_{: .text-green-100} of the argon atom in the `AddSpecies` node to 1000.
- Set the _Temperature_{: .text-green-100} of the configuration 85 K to match that of the experimental measurement.

Because we've changed some settings, we need to hit the _Regenerate_{: .text-green-100} button to recreate our initial configuration.

> You can regenerate a configuration at any time, but bear in mind that other quantities calculated by modules may also need to be cleared. Remove all of this data to begin a 'clean' run from the _Simulation_{: .text-green-100} menu (_Simulation→Clear Module Data_{: .text-green-100}).
{: .tip}

[Previous Step](step2.md){: .btn }   [Next Step](step4.md){: .btn .right}
