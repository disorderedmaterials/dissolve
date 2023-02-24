---
title: Main Forcefield
description: The Forcefield tab
weight: 1
---

The purpose of the Forcefield tab is to provide access to general forcefield terms used within the simulation, covering atom types and master terms.

### Atom Types & Pair Potentials

{{< cimage src="../forcefieldtab_atomtypes.png" caption="Forcefield tab showing current atom types and potentials" width=90% >}}

Basic editing of atom types (addition, duplication etc.) is provided by this tab, as well as control over the generated pair potentials. Of particular importance are the options to control the cutoff distance of the potential and the source of atom charges in the generated potentials.

#### Atom Types List

The left-most list shows the current atom types defined for the simulation, including their potential parameters - charge and short-range (SR) definition. Both may be edited freely from here.

#### Pair Potentials

Pair potentials are generated from the potential parameters defined for the atom types, in conjunction with the _Charge Source_ option. If the current simulation hasn't already been prepared or run then clicking the _Regenerate All_ button will recalculate all pair potentials.

Selecting a pair potential from the list plots the tabulated energy and force functions.

#### Control

The pair potential _Range_ determines the extent over which pairwise interactions between atoms are effective in the simulation. Typically this is between 10 to 15 Angstroms, but may need to be made larger in extreme cases. It is important to note that the longer the pair potential range, the longer it takes to calculate total system energies, and the more time it will take your simulation to run.

The _Delta_ value controls the spacing between points, in Angstroms, in the tabulated pair potentials. The default value is sufficient for most cases and should not normally need to be changed.

#### Short-Range Truncation

The short-range part of the potential typically goes _close_ to zero at longer distances (past 10 Angstroms) but doesn't go _exactly_ to zero, creating a discontinuity in both energy and force which will cause significant artifacts in any simulation. Thus, a suitable truncation scheme must be employed to ensure that the energy and force go smoothly to zero at the limit of the pair potential.

#### Coulomb Truncation

In an analogous manner to the short-range part of the potential, the Coulombic term must also be made to go to zero at the limit of the pair potential. 

#### Charge Source

When _Choose Automatically_ is enabled for the charge source (the default) Dissolve will make a choice over where to take atom charges for the pair potentials:

- _Atom Types_ - Tabulated pair potentials will include a Coulombic term based on the charges defined for individual atom types in the left-most list.
- _Species Atoms_ - Tabulated pair potentials will only include the short-range part of the potential - Coulomb interactions will be calculated on-the-fly using charges specified on individual species atoms.

Of the two options, _Species Atoms_ is much more flexible in terms of specifying fine-grained, realistic charge distributions, but is slower to calculate.

When attempting to choose which scheme to use Dissolve may fail to make a definite choice and refuse to proceed. This can happen if there are a suspicious number of atom types or species atoms with exactly zero charge, for instance. In those cases you must intervene and untick _Choose Automatically_ and make the choice yourself. Note that Dissolve may still complain about your choice and refuse to run, in which case you must evaluate the situation and, if things are as you want them, select the _Force Choice_ option.

### Master Terms

{{< cimage src="../forcefieldtab_master.png" caption="Forcefield tab showing current master terms" width=90% >}}

Any defined master terms are shown here with their parameters, split into the different intramolecular interaction types. Parameters may be freely edited here (note that they cannot be edited from a species which references the master term).