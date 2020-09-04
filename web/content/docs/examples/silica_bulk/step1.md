---
title: Step 1 - Set up Atomic Species
---


Somewhat obviously, we need two atomic species for this simulation - an oxygen, and a silicon. First, however, we'll create some suitable atom types by hand.

{{< action type="tabs" text="**Forcefield** tab, **Atom Types** section">}}
{{< step text="Click the **[+] Add** button located at the top-right" >}}
{{< step text="Select silicon in the periodic table dialog" >}}
{{< action type="edit" text="Change the _Charge_ of the new atom type to `2`, the _SR Form_ to `LJ`, _Parameter 1_ (Lennard-Jones epsilon) to `0.175`, and _Parameter 2_ (Lennard-Jones sigma) to `1.03`">}}
{{< step text="Click the **[+] Add** button again, and choose oxygen from the periodic table" >}}

{{< action type="edit" text="Change the _Charge_ of the oxygen atom type to `-1`, the _SR Form_ to `LJ`, _Parameter 1_ (Lennard-Jones epsilon) to `0.165`, and _Parameter 2_ (Lennard-Jones sigma) to `3.5`">}}

{{< tip text="Atom types created in this way are named after the symbol of the relevant element." >}}
	
Now let's generate our atomic species, assign the relevant atom types, and create a basic site for each. First, the silicon:

{{< action type="menu" text="Species &#8680; Create... &#8680; Atomic..." >}}
{{< step text=" Select silicon from the periodic table" >}}
{{< action type="tabs" text=" **Si** species tab, **Contents & Forcefield** section" >}}
{{< step text="In the list of _Atoms_ change the _AtomType_ for the atom to `Si`" >}}
{{< action type="tabs" text="**Si** species tab, **Sites** section" >}}
{{< step text="Select the silicon atom in the viewer, and click _Create_ in the toolbar to create a site" >}}

And now, the oxygen:

{{< action type="menu" text="Species &#8680; Create... &#8680; Atomic..." >}}
{{< step text="Select oxygen from the periodic table" >}}
{{< action type="tabs" text="**O** species tab, **Contents & Forcefield** section" >}}
{{< step text="In the list of _Atoms_ change the _AtomType_ for the atom to `O`" >}}
{{< action type="tabs" text="**O** species tab, **Analysis Sites** section" >}}
{{< step text="Select the oxygen atom in the viewer, and click _Create_ in the toolbar to create a site" >}}


{{< button pos="left" text="Back to Overview" path="../">}}
{{< button pos="right" text="Next Step" path="../step2/">}}
