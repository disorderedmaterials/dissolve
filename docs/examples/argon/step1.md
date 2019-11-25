---
title: Step 1 - Creating a Species
grand_parent: Examples
parent: Liquid Argon
nav_order: 1
---
# Liquid Argon

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
