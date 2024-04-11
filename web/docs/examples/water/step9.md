---
title: Step 9 - Setting up Analysis
type: docs
weight: 11
---


Having now generated and refined a simulation so that it is consistent with experimental data, we can finally get round to the main goal of any total scattering experiment - understanding the structure of the system. Exactly what questions you want to ask depends on the what you really want to know about your system, and what analysis you will want to run on your simulation.

The process of refining a simulation against experimental structure factors means that we already have access to the atomic partial and total radial distribution functions. However, usually there are more interesting properties to know - for liquid water, we may wish to know the centre-of-mass radial distribution function (i.e. a g(r) between molecules) instead. We may want to quantify the geometry of the hydrogen bond interactions in the system, or we might be interested in the three-dimensional distribution of water molecules about a central, reference water molecule. The final "production" stage involves calculating these quantities and drawing some meaningful conclusions from them.

To achieve this we need to define one or more 'sites' on our water molecule. Sites are used in all of Dissolve's analysis routines, and represent specific atoms, or averages of collections of atoms to be used as points of reference in the calculation of quantities. There are built-in analysis modules to calculate common quantities of interest, but Dissolve also allows custom analysis routines to be defined. Here, we shall only concern ourselves with Dissolve's built-in functionality.

### Defining Sites

Sites are defined within species and basically represent instructions for calculating a reference point (and possibly axes) for any molecule of that type in the simulation. Sites can be found on the relevant species tab:

{{< action type="key" text="Pause the simulation with **Esc**" />}}
{{< action type="tabs" >}}Open the {{< gui-tab text="Water" type="Species" >}} species tab and go to the {{<gui-button icon="site" text="Sites">}} section {{< /action >}}

You'll see in this section a list of defined sites (currently none) for your water species, along with a viewer of the species on the right which can be used to interactively select atoms for the site origin, or to define axes. It also illustrates the position and, if defined, the axes for the site.

### Next Steps

The following subsections will describe how to set up several flavours of analysis in separate layers. Feel free to add all of them, or just a selection, to your simulation before moving on to [Step 10](../step10/) and starting it up again to see the results.

* * *
{{< button pos="left" text="Previous Step" path="step8/">}}
{{< button pos="right" text="Next Step" path="step9a/">}}
