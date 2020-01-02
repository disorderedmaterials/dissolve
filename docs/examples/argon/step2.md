---
title: Step 2 - Set up a Forcefield
grand_parent: Examples
parent: Liquid Argon
nav_order: 2
---
# Liquid Argon

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
