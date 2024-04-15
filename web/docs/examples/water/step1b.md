---
title: Step 1b - Add Isotopologues
type: docs
weight: 3
---

We have three experimental datasets in this example - H<sub>2</sub>O, D<sub>2</sub>O, and a 50:50 mix of the two. The natural isotopologue (H<sub>2</sub>O) is defined automatically by Dissolve, so we don't need to add it by hand. We will need to add a new one for D<sub>2</sub>O, but that is all. Mixtures (e.g. the H<sub>2</sub>O:D<sub>2</sub>O 50:50 sample) are created by blending isopologues, rather than defining specific isotopologues for the desired isotope ratios, and will be done when we set up the {{< module "NeutronSQ" >}} modules.

{{< action type="tabs" >}}Go to the {{< gui-tab type="species"  text="Water" >}} species tab, and click the {{<gui-tab text="Isotopologues" type="threeSpecies">}} section{{< /action >}}
{{< action type="mouse" >}}Click {{< gui-button text="Add" icon="add" >}}{{< /action >}}
{{< step text="This will create a new isotopologue containing every atom type in the species, defaulting to the natural isotope mix" />}}
{{< action type="mouse" >}} Change the isotope for the HW atom type from `Natural (bc = -3.739 fm)` to `2 (bc = 6.671 fm)`" {{< /action >}}
{{< action type="edit" text="Change the name of the isotopologue to 'Deuterated' (double-click on the name to do so)" />}}

{{< tip text="Users familiar with EPSR might expect to also set any exchangeable atoms here in the isotopologue definitions, but in Dissolve this is handled by specific settings in individual modules." />}}

* * *
{{< button pos="left" text="Previous Step" path="step1a">}}
{{< button pos="right" text="Next Step" path="step2">}}
