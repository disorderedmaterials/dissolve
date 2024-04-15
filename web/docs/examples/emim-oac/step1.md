---
title: Step 1 - Create the Ionic Liquid Species
type: docs
weight: 1
---

We need to create the cation and anion species for our simulation. Here we'll just import them from some existing xyz coordinate files that we just happen to have laying around.

### 1-Ethyl-3-methylimidazolium Cation

First, the 1-ethyl-3-methylimidazolium cation, which we'll call "EMIM" for short from here on in:

{{< action type="menu" text="Species &#8680; Import &#8680; From XYZ..." />}}
{{< step text="Choose the `emim.xyz` file." />}}
{{<action type="edit">}}Rename the species to {{< gui-tab type="species" text="EMIM" >}} by double-clicking on the tab name{{</action>}}

Once you've selected the xyz file Dissolve will allow you to perform any last minute edits to the structure should you need to. The principal reason might be to correct the bonding between atoms, which Dissolve automatically calculates when the xyz file is read in.

The structure in the file is fine as-is, so:

{{< action type="mouse" >}}Exit the species editor by clicking {{< gui-button text="OK" >}}{{< /action >}}

{{< tip text="Once a species has been created you cannot add or remove atoms or bonds, as these now represent immutable data in your simulation. If you need to change something, you'll have to create a new species." />}}

The creation of the species is now complete, and should look like this:

{{< cimage src="../emim.png" caption="The imported EMIM cation structure" >}}

For the uninitiated, the EMIM cation is a five-membered imidazole which has been alkylated on both nitrogen atoms (with an ethyl and a methyl group respectively), resulting in a +1 overall charge.

### Acetate Anion

Second, the acetate anion, which we'll refer to as "OAc".

{{< action type="menu" text="Species &#8680; Import &#8680; From XYZ..." />}}
{{< step text="Choose the `oac.xyz` file." />}}
{{< action type="mouse">}}Again, the structure in the file is fine as-is, so click {{< gui-button text="OK" >}} to accept the structure.{{< /step >}}
{{<action type="edit">}}Rename the species to {{< gui-tab type="species" text="Acetate" >}} by double-clicking on the tab name{{</action>}}

{{< cimage src="../oac.png" caption="The imported OAc anion structure" >}}

And there you have it - acetic acid which has been deprotonated to give us an anion with -1 overall charge to balance out our cation.

* * *
{{< button pos="left" text="Back to Overview" path="">}}
{{< button pos="right" text="Next Step" path="step2/">}}
