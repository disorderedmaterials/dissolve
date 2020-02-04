---
title: Step 1b - Add Isotopologues
parent: Liquid Water
grand_parent: Examples
---
# Liquid Water

## 1b. Add Isotopologues

We have three experimental datasets in this example - H<sub>2</sub>O, D<sub>2</sub>O, and a 50:50 mix of the two. The natural isotopologue (H<sub>2</sub>O) is defined automatically by Dissolve, so we don't need to add it by hand. We will need to add a new one for D<sub>2</sub>O, but that is all. Mixtures (e.g. the H<sub>2</sub>O:D<sub>2</sub>O 50:50 sample) are created by blending isopologues, rather than defining specific isotopologues for the desired isotope ratios, and will be done when we set up the [`NeutronSQ`](../../userguide/modules/neutronsq) modules.

> **Species** tab, **Isotopologues** section
{: .action .action_tabs}
> Click _Add_{: .text-green-100}
{: .step}
> Expand the entries for the 'NewIsotopologue' by clicking the [+] next to its name, or by clicking _Expand All_{: .text-green-100}
{: .step}
> Change the isotope for the HW atom type from `Natural (bc = -3.739 fm)` to `2 (bc = 6.671)`
{: .step}
> Change the name of the isotopologue to 'Deuterated' (double-click on the name to do so)
{: .step}

> Exchangeable atoms are not specified in isotopologue definitions, but instead are indicated when using the isotopologues in modules.
{: .tip}

[Back to Overview](index.md){: .btn }   [Next Step](step2.md){: .btn .right}
