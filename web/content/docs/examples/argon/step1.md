---
title: Step 1 - Creating a Species
---

We need a suitable species to represent the argon atom in our simulation.

{{< action type="menu" text="Species &#8680; Create...→Atomic" >}}
{{< step text="Select argon (Ar) from the periodic table" >}}

You will now see that a new tab called 'Ar' has appeared containing the new species, showing a single argon atom located at (0,0,0). If you go to the **Contents & Forcefield** section you can view the details of the species contents (which is not a lot!). Our argon atom currently has no atom type assigned to it, but we will remedy this in the next step. All atoms **must** be assigned an atom type before the simulation can proceed.

{{< tip text="An atom type contains descriptive interaction parameters for a particular type of atom, and optionally an associated atomic charge. They describe the core interatomic interactions in your system, and define the pair potentials between atoms." >}}

Note also the presence of the **Isotopologues** section which allows us to define isotopes for specific atom types, and which we'll return to later.

{{< tip text="Isotopologue definitions depend on atom types, so all atom types should be defined before setting up isotopologues." >}}

<a class="btn left" href="/docs/examples/argon/">Back to Overview</a>
<a class="btn right" href="../step2/">Next Step</a>

