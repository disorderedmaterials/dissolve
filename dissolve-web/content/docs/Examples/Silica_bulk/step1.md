---
title: Step 1 - Set up Atomic Species
grand_parent: Examples
---

## 1. Set up Atomic Species

Somewhat obviously, we need two atomic species for this simulation - an oxygen, and a silicon. First, however, we'll create some suitable atom types by hand.

> **Forcefield** tab, **Atom Types** section
{: .action .action_tabs}
> Click the _[+] Add_{: .text-green-100} button located at the top-right
{: .step}
> Select silicon in the periodic table dialog
{: .step}
> Change the _Charge_ of the new atom type to `2`, the _SR Form_ to `LJ`, _Parameter 1_ (Lennard-Jones epsilon) to `0.175`, and _Parameter 2_ (Lennard-Jones sigma) to `1.03`
{: .action .action_edit}
> Click the _[+] Add_{: .text-green-100} button again, and choose oxygen from the periodic table
{: .step}
> Change the _Charge_ of the oxygen atom type to `-1`, the _SR Form_ to `LJ`, _Parameter 1_ (Lennard-Jones epsilon) to `0.165`, and _Parameter 2_ (Lennard-Jones sigma) to `3.5`
{: .action .action_edit}

> Atom types created in this way are named after the symbol of the relevant element.
{: .tip}
	
Now let's generate our atomic species, assign the relevant atom types, and create a basic site for each. First, the silicon:

> Species &#8680; Create... &#8680; Atomic...
{: .action .action_menu}
> Select silicon from the periodic table
{: .step}
> **Si** species tab, **Contents & Forcefield** section
{: .action .action_tabs}
> In the list of _Atoms_ change the _AtomType_ for the atom to `Si`
{: .step}
> **Si** species tab, **Sites** section
{: .action .action_tabs}
> Select the silicon atom in the viewer, and click _Create_ in the toolbar to create a site
{: .step}

And now, the oxygen:

> Species &#8680; Create... &#8680; Atomic...
{: .action .action_menu}
> Select oxygen from the periodic table
{: .step}
> **O** species tab, **Contents & Forcefield** section
{: .action .action_tabs}
> In the list of _Atoms_ change the _AtomType_ for the atom to `O`
{: .step}
> **O** species tab, **Analysis Sites** section
{: .action .action_tabs}
> Select the oxygen atom in the viewer, and click _Create_ in the toolbar to create a site
{: .step}

[Back to Overview](/docs/examples/silica_bulk/){: .btn }   [Next Step](/docs/examples/silica_bulk/step2){: .btn .right}
