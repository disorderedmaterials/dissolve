---
title: Step 2 - Set up a Forcefield
type: docs
---

To simulate any system we need to supply suitable parameters to describe the interactions between atoms, both internally within the same species and externally between atoms of the same (and different) species. The {{< gui-tab type="ff"  >}} tab is always available, and summarises the atom types used over all defined species and the resulting pair potentials, as well as master intramolecular definitions (which we aren't using in this example). You can also set other important options, such as the range of the generated pair potentials and the truncation schemes.

We'll now describe the interactions for your argon species by taking the terms from one of Dissolve's [built-in forcefields]({{< ref "ff" >}}).

{{< action type="tabs" >}}Go to the {{< gui-tab type="species"  text="Ar" >}} species tab{{< /action >}}

{{< step text="By selecting the tab containing the Argon species you are making it 'active', and so it will be the target of any actions triggered from the _Species_ menu." />}}

{{< action type="menu" text="Species &#8680; Add Forcefield Terms..." />}}

{{< step >}}From the available forcefields choose `OPLSAA2005/NobleGases` and click {{< gui-button text="Next" icon="general_arrowright" >}}{{< /step >}}

{{< step >}}Here we choose how to assign atom types to the species - the default option of _Determine atom types for all atoms_ uses connectivity descriptions in the forcefield to try and automatically choose which type to use. Leave this option selected and click {{< gui-button text="Next" icon="general_arrowright" >}}{{< /step >}}  

{{< step >}}There will be no conflicts between the proposed atom types and existing ones defined in the main simulation (since there aren't any) so click {{< gui-button text="Finish" icon="general_true" >}} to complete the wizard{{< /step >}}

{{< tip text="For more complicated (molecular) species the wizard also handles how _intra_-molecular terms are generated. You also may have to deal with potential naming conflicts with existing terms in the main simulation, which the _Add Forcefield Terms_ wizard will help you resolve." />}}

If you now return to the {{< gui-tab type="ff" text="Forcefield" >}} tab you will see in the **Atom Types** section that we have an atom type for our argon atom which contains the necessary interaction parameters. The short range type (_SR Form_ in the table) specifies the functional form of the van der Waals interaction parameters for each atom type, which in our case is `LJGeometric` indicating that the parameters reflect a standard Lennard-Jones potential utilising [geometric combination rules]({{< ref "short-range" >}}). Those parameters (`epsilon` and `sigma`) are shown in the _SR Parameters_ column and describe how our argon atoms will interact with each other in the simulation.

{{< warn text="Atom types are strictly associated with a chemical element, and can only be applied to atoms of the same element." />}}

By default, Dissolve will generate all the necessary pair potentials for the current set of atom types automatically - these are listed in the **Pair Potentials** section where you can also select individual potentials and visualise them in the associated plot.

Before we move on we need to set an option related to the charges to use. Along with van der Waals parameters atomic charges are the second important component in describing interactions between atoms. In Dissolve these charges can either be taken from the atom types and included in the tabulated pair potentials, or located on species atoms and calculated analytically. Dissolve automatically detects the most appropriate scheme but is wary of systems or species where there are too many atoms with zero charge, as is the case here. As such, we must force a choice - in the **Charge Source** group at the top-right of the {{< gui-tab type="ff"  >}} tab deselect the _Choose Automatically_ option and enable the _Force Choice_ option below it. The actual charge source (_Atom Types_ vs _Species Atoms_) is unimportant since charges are zero in both, but the default option is faster.


* * *
{{< button pos="left" text="Previous Step" path="step1/">}}
{{< button pos="right" text="Next Step" path="step3/">}}
