---
title: Step 9 - Setting up Analysis
parent: Liquid Water
grand_parent: Examples
---
# Liquid Water

## 9. Setting up Analysis

Simply by running the simulation and refining it we have a set of calculate total and partial g(r) for each atom type pair, which already gives us a great deal of information about the system. However, it is typically the case where the questionf of scientific interest are far more specific. In the case of liquid water, for instance, we may wish to know the centre-of-mass radial distribution function (i.e. a g(r) between molecules), the correlation between H&bull;&bull;&bull;O distance and O-H&bull;&bull;&bull;O angle where hydrogen bonds are present, or the three-dimensional distribution of molecules around a central molecule (i.e. a 3D radial distribution function, rather than one which is spherically averaged).

To achieve this we need to define one or more 'sites' on our water molecule. Sites represent specific atoms, or averages of collections of atoms, or define axes using atoms in the species in order to provide points of reference for all of Dissolve's anlaysis routines. Moreover, while there a built-in analysis modules to calculate common properties of interest, Dissolve also allows custom analysis routines to be defined. Here, we shall only concern ourselves with Dissolve's built-in functionality.

### Defining Sites

Sites are defined on a per-species basis and in the basic case comprise of a list of species atom indices whose positions should be averaged in order to define a coordinate that represents the site. They may optionally contain lists of species atom indices to average and use as defining directions for _x_ and _y_ axes (the _z_ axis being generated automatically from those).

Sites can be defined from the parent species tab:

> Pause the simulation with **Esc**
{: .step .action_key}
> **Water** species tab
{: .action .action_tabs}
> Open the **Analysis Sites** section
{: .step }

You'll see in this section a list of defined sites (currently none) for your water species, along with a viewer of the species on the right which can be used to interactively select atoms for the site origin, or to define axes. It also illustrates the position and, if defined, the axes for the site.

### Next Steps

The following subsections will describe how to set up several flavours of analysis in separate layers. Feel free to add all of them, or just a selection, to your simulation before moving on to [Step 10](step10.md) and starting it up again to see the results.

[Previous Step](step8.md){: .btn }   [Next Step](step9a.md){: .btn .right}
