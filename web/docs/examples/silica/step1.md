---
title: Step 1 - Set up Atom Types
type: docs
---


We'll be setting up our silica system using atomic (ionic) species, rather than creating an explicitly bound network. So, we need two atomic species for this simulation - an oxygen, and a silicon. First, however, we'll create some suitable atom types by hand - this is instead of applying a built-in forcefield or loading one in from elsewhere.

{{< action type="tabs">}}Select the {{<gui-tab type="ff" text="Forcefield">}} tab{{</action>}}
{{< action type="mouse" >}}In the **Atom Types** group, click the {{< gui-button icon="add" text="Add" >}} button located at the top-right{{< /action >}}
{{< action type="mouse" text="Select silicon in the periodic table dialog" />}}

{{< tip text="Atom types created in this way are named after the symbol of the relevant element." />}}

This creates an "empty" atom type in the table, and which we'll now set some useful interaction parameters.

{{< action type="edit" text="Change the \"Charge\" of the new atom type to `2`" />}}
{{< action type="edit" text="Change the \"SR Form\" to `LJ` to request a Lennard-Jones short-range potential for the atom type" />}}
{{< action type="edit" text="In the \"SR Parameters\" box enter `epsilon=0.175 sigma=1.03`" />}}

We'll now use the same process to create an oxygen atom type:

{{< action type="mouse" >}}Click the {{< gui-button icon="add" text="Add" >}} button again, and choose oxygen from the periodic table {{< /action >}}

{{< action type="edit" text="Change the \"Charge\" of the oxygen atom type to `-1`" />}}
{{< action type="edit" text="Change the \"SR Form\" to `LJ`" />}}
{{< action type="edit" text="Set the \"SR Parameters\" to `epsilon=0.165 sigma=3.5`" />}}

You might ask where these values for the short-range interaction parameters come from, and why the charges don't reflect what you might expect to be "formal" oxidation state charges for the ions.  Well, those are both good questions, and the real answer is that there are going to be a variety of combinations of Lennard-Jones epsilon, sigma, and atomic charges that give reasonable structures for silica, and those listed here are good enough for a starting point for us.  There are no doubt other forcefields in the literature which suggest different parameters for an atomic/ionic representation of the system - you are free to try those if you wish!

* * *
{{< button pos="left" text="Back to Overview" path="">}}
{{< button pos="right" text="Next Step" path="step2/">}}
