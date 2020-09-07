---
title: Step 3 - Add a Configuration
---


A configuration represents the target system using one or more of the available species. Our next step is to create a configuration that is consistent with the experimentally-measured system investigated in the original paper.

Configurations in Dissolve are created from a sequence of instructions that define box shape and size, the populations of species to add etc. This permits Dissolve to automatically regenerate configurations when it needs to, and is particularly useful when screening structures with variable parameters (density, composition, pore size etc.).

Dissolve provides several standard configuration generators for common-use cases, one of which we will use now.

{{< action type="menu" text="Configuration &#8680; Create... &#8680; Simple random mix" >}}
{{< step text="A random mix can contain any number of species - we only have one, so select the argon species" >}}


You will now see that the **Generator** section has been populated with several steps, and a new configuration of atomic coordinates has been created based on these steps.  We'll go through these one-by-one in the order they appear. To see the settings for any step, click the cog icon in its bottom-right corner.

| Node | Purpose |
|------|---------|
| [`Parameters`](../../userguide/nodes/parameters) | Defines one or more numerical parameters that can be referenced by other nodes in the generator. We have only one (floating point) value called `rho` which defines the density of the configuration. Note that `rho` is just a number and has no associated units - these are specified in the [`AddSpecies`](../../userguide/nodes/addspecies) node. |
| [`Box`](../../userguide/nodes/box)   | Defines side lengths and angles for the periodic box. Note that _relative_ lengths can be given, and the box expanded automatically (when adding species, for instance) to achieve some specific density. |
| [`AddSpecies`](../../userguide/nodes/addspecies) | Adds a number of copies of the specified species to the configuration. The option **ScaleBox** controls whether the box volume should be scaled to give the density specified once the species has been added, and is most useful when supplying relative box lengths. If turned off, the box would remain at its current size (defaulting to 1.0 &#8491;<sup>3</sup>) but the full population of molecules will still be added. |

In the `AddSpecies` node note how we have reference the `rho` parameter for the density. Many numerical values for nodes 

{{< tip text="Many numerical options can be given in the form of equations referencing variables such as those set in a `Parameters` node. A green tick indicates if the equation is valid.">}}

Let's change the settings of our configuration's generator to give us a suitable system.

{{< action type="settings" text="Open the settings for the `Parameters` node" >}}
{{< step text="Change the value of the \"rho\" variable to 0.0213 (which is our target density in units of atoms/A3)">}}
{{< action type="settings" text="Open the settings for the `AddSpecies` node" >}}
{{< step text="Check that the units for the **Density** are set to `atoms/A3`">}}
{{< step text="Increase the **Population** to 1000">}}

Finally, we need to set the correct temperature for the configuration.

{{< action type="edit" text="Set the **Temperature** to 85 K to match that of the experimental measurement" >}}

Because we've changed some settings, we need to hit the **Regenerate** button to recreate our initial configuration.

{{< tip text="You can regenerate a configuration at any time, but bear in mind that other quantities calculated by modules may also need to be cleared. Remove all of this data to begin a 'clean' run with the **Clear Module Data** option in the **Simulation** menu.">}}

{{< button pos="left" text="Previous Step" path="../step2/">}}
{{< button pos="right" text="Next Step" path="../step4/">}}
