---
title: Step 4 - Results
type: docs
weight: 1
---

Now that the wizard has completed you'll see that the process has yielded a species with a full forcefield assignment, ready to do your simulated bidding. That was easy, wasn't it?

Some things to consider / think about when using this approach:

- Many simulators (including myself) have a lot of faith in the underlying parameter set (OPLS-AA) used by LigParGen, so it should give usable results for a great many molecules. However, you may find systems that do not work, so don't treat it as a guarantee.
- Charges are assigned to species atoms, not atom types, so you may need to ensure that this particular source of charges is selected / forced in the {{< gui-tab type="ff" text="Forcefield" >}}.
- If you use LigParGen, please cite it as it's a fantastic resource. The necessary references are produced below for convenience.

References:
1. "Potential energy functions for atomic-level simulations of water and organic and biomolecular systems.", W. L. Jorgensen and J. Tirado-Rives, _Proc. Nat. Acad. Sci. USA_ **102**, 6665-6670 (2005).

2. "1.14\*CM1A-LBCC: Localized Bond-Charge Corrected CM1A Charges for Condensed-Phase Simulations.", L. S. Dodda, J. Z. Vilseck, J. Tirado-Rives, and W. L. Jorgensen, _Phys. Chem. B_ **121** (15), 3864-3870 (2017).

3. "LigParGen web server: An automatic OPLS-AA parameter generator for organic ligands.", L. S. Dodda, I. Cabeza de Vaca, J. Tirado-Rives, and W. L. Jorgensen, _Nucleic Acids Research_ **45** (W1), W331-W336 (2017).

* * *
{{< button pos="left" text="Previous Step" path="step3/">}}
{{< button pos="right" text="Exercises" path= "exercises/">}}
