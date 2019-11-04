---
title: Step 6 - Refine the Potential
parent: Liquid Water
grand_parent: Examples
---
# Liquid Water

## 6. Refine the Potential

Let's briefly recap what we've done so far:

1. Set up a liquid water system based on a literature forcefield (`SPC/Fw`)
2. Equilibrated the system and made an initial structural comparison with experimental data
2. Adjusted the intramolecular geometry of the water molecule in order to better match the experimental data

Our agreement with experiment is OK, but it is possible to make it even better by modifying the _inter_-atomic interaction parameters contained in the atom types. However, generally this is not to be attempted by hand in all but the simplest of cases, as the effects of adjusting the interatomic are seldom as obvious as those for _intra_-molecular parameters. Also, for even a modestly-complicated system the number of adjustable parameters is simply too large to tackle with manual fitting.

Here we'll employ the [`EPSR`](/modules/epsr) module in order to adjust the interatomic potentials automatically to give better agreement with the experimental reference data.

> Layer &#8680; Create... &#8680; Refinement... &#8680; Standard EPSR
{: .action .action_menu}

Our new layer contains only the [`EPSR`](/modules/epsr) module, and which Dissolve has set up with sensible targets and defaults. Double-click the [`EPSR`](/modules/epsr) module to open it up as a tab - we'll need to explore the various graphs as we proceed, but for now let's check the set-up of the module.

> **Refine (EPSR)** tab
{: .action .action_tabs}
> Open the **Calculation** settings group
{: .action .action_settings}
> An initial value for **EReq** has been set - thi
{: .step}




There's a lot to explain here, so set the simulation running again to begin the refeand then 

[Previous Step](step5.md){: .btn }   [Next Step](step7.md){: .btn .right}
