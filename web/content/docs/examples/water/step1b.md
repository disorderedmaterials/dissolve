---
title: Step 1b - Add Isotopologues
weight: 3
---

We have three experimental datasets in this example - H<sub>2</sub>O, D<sub>2</sub>O, and a 50:50 mix of the two. The natural isotopologue (H<sub>2</sub>O) is defined automatically by Dissolve, so we don't need to add it by hand. We will need to add a new one for D<sub>2</sub>O, but that is all. Mixtures (e.g. the H<sub>2</sub>O:D<sub>2</sub>O 50:50 sample) are created by blending isopologues, rather than defining specific isotopologues for the desired isotope ratios, and will be done when we set up the [`NeutronSQ`](../../userguide/modules/neutronsq) modules.

{{< action type="tabs" text="**Species** tab, **Isotopologues** section" >}}
{{< step text="Click **Add**" >}}
{{< step text="Expand the entries for the 'NewIsotopologue' by clicking the [+] next to its name, or by clicking **Expand All**" >}}
{{< step text="Change the isotope for the HW atom type from `Natural (bc = -3.739 fm)` to `2 (bc = 6.671)`" >}}
{{< step text="Change the name of the isotopologue to 'Deuterated' (double-click on the name to do so)" >}}

{{< tip text="Exchangeable atoms are not specified in isotopologue definitions, but instead are indicated when using the isotopologues in modules." >}}

{{< button pos="left" text="Previous Step" path= "../step1a">}}
{{< button pos="right" text="Next Step" path= "../step2">}}
