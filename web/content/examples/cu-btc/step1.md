---
title: Step 1 - Import the CIF
type: docs
weight: 1
---

Our first task is to import the CIF file - Dissolve has a wizard to help you do this, and which also allows some tidying of the imported coordinates, replication of the unit cell to create a supercell etc.

{{< action type="menu" text="Species &#8680; Import &#8680; From CIF..." />}}
{{< action type="mouse" >}}Select the relevant CIF file (7108574.cif) and then click {{< gui-button text="Next" >}}{{< /action >}}
{{< step >}}Dissolve will parse the CIF and display some basic information from it such as the authors (Peikert, Hoffmann, and Froba), publication reference, and space group (Fm-3m). Click {{< gui-button text="Next" >}} when you're ready{{< /step >}}

{{< tip text="If the space group can't be detected from the file, or Dissolve isn't sure which one to use, the wizard will show an additional page asking for clarification." />}}

The wizard will now be showing you the unit cell and its contents as determined from the CIF (you might need to zoom out or rotate the view a bit to see something meaningful). On this page you have some broad choices on the tolerance for removing overlapping atoms (which arise from defined symmetry generators creating symmetry-identical copies), for how connectivity between atoms is calculated (either by Dissolve or from defined bond distances in the CIF) and which structural assemblies / groups (typically arising from disordered moieties in the determined crystal structure) are used.

{{< cimage src="../cu-btc-default.png" caption="Basic Cu-BTC unit cell" >}}

The default options get us most of the way there, but it's instructive to take a look at the **Assemblies** that were detected in the CIF. An assembly is a set of symmetry-unique atoms that have one or more configurational states (i.e. they have different positions) and can represent disorder, different conformers of molecules, or even completely different molecules within the same unit cell. If you look at Peikert, Hoffman, and Froba's [original paper](https://doi.org/10.1039/C2CC36220A) you'll see that the focus is on an amino derivative of the BTC linkers, and both the original Cu-BTC and the amino-substituted structure are represented in the CIF. The default assembly/group combination of `Global/Default`, `B/Default` and `A/1` gives the normal Cu-BTC structure, while disabling `A/1` and enabling `B/2` and `C/2` gives you the amino-substituted version.

Before we proceed, make sure that the default Cu-BTC groups (`Global/Default`, `B/Default`, and `A/1`) are the only ones enabled.

Now, if you look closely at the crystal structure you'll see "extra" oxygen atoms attached to the copper sites:

{{< cimage src="../cu-btc-default-zoom.png" caption="Dangling oxygens on Cu sites" >}}

These are in fact from coordinated water molecules present when the crystal structure was determined - the water hydrogen atoms are not present in the CIF data. We will remove these oxygen atoms since we want a perfectly "dry" unit cell.

{{< action type="mouse" >}}Click {{< gui-button text="Next" >}} in the wizard to move to the _Clean Structure_ page{{< /action >}}

The _Clean Structure_ page has several options for cleaning up various aspects of the structure we currently have, for example removing free atoms/ions. We want to use the option to remove water molecules, which will also remove terminal oxygen atoms based on the assumption that hydrogen positions were not available in the CIF. So:

{{< action type="mouse" >}}Enable the _Water and coordinated oxygen_ option{{< /action >}}

You should see those terminal oxygen atoms disappear from the structure, leaving us with a "pure" Cu-BTC framework. There are a couple of pages left in the wizard which allow us to create a supercell from the unit cell,

{{< action type="mouse" >}}Click {{< gui-button text="Next" >}} in the wizard to move to the _Create Supercell_ page. For production simulations you will probably want to work on something bigger than the unit cell, but here we'll keep things limited in size{{< /action >}}
{{< action type="mouse" >}}Click {{< gui-button text="Next" >}} to move to the final page of the wizard, the _Species Partitioning_ page. We have a couple of options here of what to actually generate in terms of the species. The default option _Periodic Framework_ creates a species encompassing the entire periodic (super) cell, which is what we want{{< /action >}}
{{< action type="mouse" >}}Click {{< gui-button text="Finish" >}} to complete the wizard and generate the Cu-BTC species{{< /action >}}

Our new species not only contains the atoms of the framework, but also the unit cell (or supercell) definition - later on we can use this as our configuration's box rather than defining it ourselves.

* * *
{{< button pos="left" text="Back to Overview" path="./">}}
{{< button pos="right" text="Next Step" path="step2">}}
