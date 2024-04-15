---
title: Step 1 - Creating a Species
type: docs
---

We need a suitable species to represent the argon atom in our simulation.

{{< action type="menu" text="Species &#8680; Create &#8680; Atomic..." />}}
{{< action type="mouse" text="Select argon (Ar) from the periodic table" />}}

You will now see that a new tab called {{<gui-tab type="species" text="Ar">}} has appeared containing the new species. In the {{<gui-tab type="species" text="Atoms">}} section on the left you can see the single argon atom located at (0,0,0). and underneath that the summary of intramolecular terms holding your species together which, for a single atom, isn't a lot! Our argon atom currently has no atom type assigned to it (the "AtomType" (AT) column in the table is empty), but we will remedy this in the next step. All atoms **must** be assigned an atom type before the simulation can proceed.

{{< tip text="An atom type contains descriptive interaction parameters for a particular type of atom, and optionally an associated atomic charge. They describe the core interatomic interactions in your system, and define the pair potentials between atoms." />}}

Note also the presence of the {{<gui-tab text="Isotopologues" type="threeSpecies">}} section which allows us to define isotopes for specific atom types, and which we'll return to later.

{{< tip text="Isotopologue definitions depend on atom types, so all atom types should be defined before setting up isotopologues." />}}

* * *
{{< button pos="left" text="Back to Overview" path="./">}}
{{< button pos="right" text="Next Step" path="step2">}}
