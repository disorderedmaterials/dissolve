---
title: Step 3 - Add a Configuration
type: docs
---


A configuration represents the target system using one or more of the available species. Our next step is to create a configuration that is consistent with the experimentally-measured system investigated in the original paper.

Configurations in Dissolve are created from a sequence of instructions that define box shape and size, the populations of species to add etc. This permits Dissolve to automatically regenerate configurations when it needs to, and is particularly useful when screening structures with variable parameters (density, composition, pore size etc.).

Dissolve has a wizard to guide you through the process of preparing standard configurations:

{{< action type="menu" text="Configuration &#8680; Create..." />}}
{{< action type="mouse" >}}
##### Target Species
The first step of the wizard is to select all of the species which will go in to it - we only have one (our argon atom) so highlight it in the list and press {{< gui-button text="Next" icon="arrowright" >}}{{< /action >}}

{{< action type="mouse" >}}
##### Configuration Type
The next choice is which type of configuration to generate. The available choices depend on the types of species selected - we can only make a random mix from the argon species, so press {{< gui-button text="Next" icon="arrowright" >}} to continue.{{< /action >}}

{{< action type="mouse" >}}
##### Box Geometry
With our basic type of configuration chosen we must now decide on the size and geometry of the periodic box (unit cell). The two options are _Fixed Geometry, Undefined Size_ (where the final size of the box will depend on the how much and at what density we add in to it) and _Fixed Geometry and Size_ where we set both explicitly now. We will use the former choice here.

When selecting _Fixed Geometry, Undefined Size_ the axis lengths given are _relative_ since they will be scaled as species are added to the box. Setting them all to `1.0` and leaving the angles at `90` (the default) results in a cubic box which should be appropriate for most simulations. Press {{< gui-button text="Next" icon="arrowright" >}} to continue.{{< /action >}}

{{< action type="edit" >}}
##### Species Information

On the final page of the wizard we set the populations of the species and, if we chose _Fixed Geometry, Undefined Size_, the required density of the system. Change the _Density_ to `0.0213` atoms/A3, then press {{< gui-button text="Finish" icon="true" >}} to complete the wizard.{{< /action >}}

You will now see that the {{< gui-button text="Generator" icon="generator" >}} section has been populated with several steps, and a new configuration of atomic coordinates has been created based on these steps.  We'll go through these one-by-one in the order they appear. To see the settings for any step, click on it to show its options in the panel immediately below the list of steps.

| Node | Purpose |
|------|---------|
| {{< node "Temperature" >}}| Defines the temperature of the configuration. |
| {{< node "Parameters" >}}| Defines one or more numerical parameters that can be referenced by other nodes in the generator. We have only one value called "rho" which defines the density of the configuration. Note that "rho" is just a number and has no associated units - these are specified in the {{< node "Add" >}} node. |
| {{< node "Box" >}} | Defines side lengths and angles for the periodic box. Note that _relative_ lengths can be given, and the box expanded automatically (when adding species, for instance) to achieve some specific density. |
| {{< node "Add" >}}| Adds a number of copies of the specified species to the configuration. The **BoxAction** option controls how/if the box volume should be scaled to give the density specified once the species has been added, and is most useful when supplying relative box lengths. If set to `None` the box would remain at its current size (defaulting to 1.0 &#8491;<sup>3</sup>) but the full population of molecules will still be added. |

In the {{< node "Add" >}} node note how we have referenced the "rho" parameter for the density.

{{< tip >}}Many numerical options can be given in the form of equations referencing variables such as those set in a {{< node "Parameters" >}} node. A green tick indicates if the equation is valid.{{< /tip >}}

Finally, we need to set the correct temperature for the configuration.

{{< action type="mouse">}}On the configuration viewer toolbar, click the {{< gui-button icon="temperature" text="Temperature" >}} button{{</action>}}
{{< action type="edit" text="Set the **Temperature** to 85 K to match that of the experimental measurement" />}}

{{< tip >}}You can recreate a configuration at any time by clicking the {{< gui-button text="Generate" icon="repeat" >}} button, but bear in mind that other quantities calculated by modules may also need to be cleared. Remove all of this data to begin a 'clean' run with the **Clear Module Data** option in the **Simulation** menu.{{< /tip >}}

* * *
{{< button pos="left" text="Previous Step" path="step2/">}}
{{< button pos="right" text="Next Step" path="step4/">}}
