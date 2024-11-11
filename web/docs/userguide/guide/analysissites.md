---
title: Analysis Sites
description: Defining points of reference for structural investigation
---

### Overview

Assuming that you have run a simulation - be it one that has just been equilibrated to give a reference simulation, one that has been created to test a "structural hypothesis", or one which has been fully refined against experimental data - at some point you will want to calculate something interesting from it in order to try and understand it at some atomic or molecular level. There is plenty of information contained in the radial distribution functions calculated by the {{< module "GR" >}} module, for sure, but something a little more targeted is usually useful.

Typically, you will want to be asking questions along the lines of:
- What is the centre-of-mass distribution of this molecule around another?
- Which molecules like to hydrogen bond to this specific OH group?
- What are the angular distributions of molecules around this ring plane?

Etcetera, etcetera. Most of these questions consider the distribution of one or more specific sites around some other specific site, where a site could be the inferred centre-of-mass or centre-of-geometry of a group of atoms, a specific atom itself, or anywhere inbetween. Dissolve requires you to define any sites of interest for individual species on the species themselves (on the [species tab]({{< ref speciestab >}})) and these can then be referenced by any of the [analysis modules]({{< ref analysis >}}) that are available.

Those of you coming from am EPSR background will perhaps be used to defining atom types specifically for the purposes of highlighting a particular "site" that you are interested in focussing on in your subsequent analysis (and may thus also be aware of the [dlputils](https://github.com/trisyoungs/dlputils) codes and why they exist) but Dissolve treats analysis as a more general problem requiring a separate set of definitions from the atom types.

### Types of Site

Dissolve currently allows sites to be defined in three ways:

#### "Static" Sites
A static site is based upon specific atoms within a molecule, reference by their integer indices. The root position can be defined by one or more atoms whose positions are either averaged uniformly (giving a centre-of-geometry position) or by their individual masses (giving a centre-of-mass position), Optionally, axes relative to the site's central position can be defined in order to allow calculation of spatial or angular (rather than purely radial) quantities.

#### "Dynamic" Sites
Dynamic sites are generated from specific elements or atom types within a molecule and are thus atomic in nature and cannot have axis definitions. They are useful when asking questions relating to a whole bunch of related atoms in a large molecule - for instance, you could define a dynamic site covering any carbon atom in benzene, and any analysis module using this site will consider all six occurrences as separate sites in the calculated quantity.

#### "Fragment" Sites
An extension to "dynamic" sites, these are based on a [NETA connectivity definition]({{< ref neta >}}) which is able to define both the origin site in terms of the connectivity of multiple atoms as well as the axes relative to that origin. A such it can be used to automatically define multiple identical sites within complicated molecules.


### Defining Sites

Site definitions are performed in the "Sites" panel of the [species tab]({{< ref speciestab >}}). In the 3D viewer select the atom or atoms which form the basis of the site origin, or which represent the atom types / elements for a dynamic site, and right-click to choose the relevant option under the "Create site from..." submenu. A new site will then appear in the list on the left and allow editing of any properties.  Note that once a site of a particular type has been defined its type cannot be changed and you will have to create a new site of the desired type.

The instance count (i.e. the multiplicity of the site) is displayed at the bottom of the sites panel, along with the number of atoms involved in each instance. For static sites there is always exactly one instance since they are unique, but for dynamic and fragment sites this number will represent the number of matches found in the molecule. All instances are highlighted in the 3D viewer by means of a box around each origin, also showing axes if they are defined.
