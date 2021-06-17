---
title: Creating Species
description: Defining, creating, and importing species
weight: 3
---

## Overview

Species are the basic building blocks of [configurations]({{< ref "configurations" >}}), and can represent individual ions, uncharged atoms, molecules, proteins, or even periodic structures such as metal organic frameworks. A species defines the coordinates of all atoms, how those atoms interact with each other (i.e. bonds, angles, torsion terms etc.), and how they interact "through space" with other atoms (through their assigned [atom types]({{< ref "atomtypes" >}}) and, optionally, atomic charges).

Terms describing the internal interactions between atoms within a species are the _intramolecular_ interactions, while those between unbound atoms (i.e. between atoms in different molecules, or atoms within the same molecule that are separated by a minimum of three bonds) are governed by the atom types and charges, and are the _intermolecular_ or _interatomic_ interactions. Combined, these represent a full _forcefield_ description for the individual species.

Forcefield descriptions for species can be entered manually, but it is generally a better idea to use other methods if at all possible. See the [Applying Forcefields]({{< ref "applyff" >}}) section for more information on the methods available.

### Isotopologues

When wishing to simulate neutron-weighted correlation functions such as total structure factors (via the [NeutronSQ module]({{< ref "neutronsq" >}}) a species can contain a number of specifications of atomic isotopes (linked to the atom types assigned to the atoms) in order to build up _isotopologues_ - isotopically-labelled variants of the same species.

{{< warn text="Once a species has been created its constituent atoms and bond connectivity are fixed and can no longer be edited. Atoms may have their coordinates adjusted, but bonding will remain unchanged." >}}
