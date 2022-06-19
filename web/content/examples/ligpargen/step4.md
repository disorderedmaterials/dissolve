---
title: Step 4 - Results
type: docs
weight: 1
---

Now that the wizard has completed you'll see that the process has yielded a species with a full forcefield assignment, ready to do your simulated bidding. That was easy, wasn't it?

We need to talk a little bit about atomic charges here. While the underlying OPLS-AA parameter set has charges associated to individual atom types, the output of LigParGen provides atomic charges located on individual species atoms. So, you will may have to tell Dissolve that it should take charges from _Species Atoms_ on the {{< gui-tab type="ff" text="Forcefield" >}} tab. You may also have to force that choice, depending on the other species you have in your simulation. Additionally those charges, when truncated to fit in a finite number of decimal places, often result in a small positive or negative charge on the species (of the order of 1.0e-4 e). You will have to ensure that your species overall charge is zero, otherwise it's likely that you'll end up with a configuration with a non-zero charge, and that also isn't allowed.

If you use LigParGen, please cite it as it's a fantastic resource. The necessary references [1-3] are produced below for convenience. Many simulators (including myself) have a lot of faith in the underlying parameter set (OPLS-AA) used by LigParGen, so it should give usable results for a great many molecules. However, you may find systems that do not work, so don't treat it as a guarantee.

References:
1. "Potential energy functions for atomic-level simulations of water and organic and biomolecular systems.", W. L. Jorgensen and J. Tirado-Rives, _Proc. Nat. Acad. Sci. USA_ **102**, 6665-6670 (2005).

2. "1.14\*CM1A-LBCC: Localized Bond-Charge Corrected CM1A Charges for Condensed-Phase Simulations.", L. S. Dodda, J. Z. Vilseck, J. Tirado-Rives, and W. L. Jorgensen, _Phys. Chem. B_ **121** (15), 3864-3870 (2017).

3. "LigParGen web server: An automatic OPLS-AA parameter generator for organic ligands.", L. S. Dodda, I. Cabeza de Vaca, J. Tirado-Rives, and W. L. Jorgensen, _Nucleic Acids Research_ **45** (W1), W331-W336 (2017).

* * *
{{< button pos="left" text="Previous Step" path="step3/">}}
{{< button pos="right" text="Exercises" path= "exercises/">}}
