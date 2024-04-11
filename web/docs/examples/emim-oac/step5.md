---
title: Step 5 - Structure Factors
type: docs
weight: 1
---

Time to create some isotopologues for our system and calculate the structure factors.

### Isotopologues

We need a partly-deuteriated isotopologue for the cation where the alkyl groups are fully deuteriated but the imidazole ring hydrogens remain as protons.

{{< action type="tabs" >}}Go to the {{< gui-tab type="species" text="EMIM" >}} species tab, **Isotopologues** section{{< /action >}}
{{< action type="mouse">}}Select the {{< gui-button icon="threeSpecies" text="Isotopologues" >}} section to create a new isotopologue definition{{< /action >}}
{{< action type="mouse">}}Click {{< gui-button icon="add" text="Add" >}} to create a new isotopologue definition{{< /action >}}
{{< action type="mouse" text="Right-click on the name of the new isotopologue and choose **Set hydrogens to** &#8680; **Deuteriated** from the context menu to set all hydrogen types to deuterium" />}}
{{< action type="edit" text="Change the isotopes for the `HCW` and `HCR` atom types back to to `Natural (bc = -3.739 fm)`" />}}
{{< action type="edit" text="Double-click the name of the isotopologue and change it to `H3D8`"/>}}

For the anion we need a fully deuteriated version:

{{< action type="tabs" >}}Go to the {{< gui-tab type="species" text="Acetate" >}} species tab, {{< gui-button icon="threeSpecies" text="Isotopologues" >}} section{{< /action >}}
{{< action type="mouse">}}Click {{< gui-button icon="add" text="Add" >}} to create a new isotopologue definition{{< /action >}}
{{< action type="edit" text="Change the isotope for the `HCA` atom type to `2 (bc = 6.671 fm)`" />}}
{{< action type="edit" text="Double-click the name of the isotopologue and change it to `D3`"/>}}

### Structure Factors

Now we can create a new layer to calculate the RDF and structure factors:

{{< action type="menu" text="Layer &#8680; Create &#8680; Correlations &#8680; RDF and Neutron S(Q)" />}}

Our reference datasets are listed in the following table - we'll refer to them in the form `X-Y` where `X` is the isotopologue of the cation and anion respectively:

| |Cation|Anion|ID|Datafile|
|-|------|-----|-------|---------|
|1|Natural|Natural|H-H|[`SLS39546-H_H.mint01`](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/emim-oac/data/SLS39546-H_H.mint01)|
|2|Natural|D3|H-D3|[`SLS39548-H_D.mint01`](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/emim-oac/data/SLS39548-H_D.mint01)|
|3|H3D8|Natural|H3D8-H|[`SLS39549-H3D8_H.mint01`](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/emim-oac/data/SLS39549-H3D8_H.mint01)|
|4|H3D8|D3|H3D8-D3|[`SLS39550-H3D8_D.mint01`](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/emim-oac/data/SLS39550-H3D8_D.mint01)|
|5|Natural|H:D3 50:50|H-50|[`SLS39567-H_50.mint01`](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/emim-oac/data/SLS39567-H_50.mint01)|
|6|H:H3D8 50:50|H:D3 50:50|50-50|[`SLS39568-50_50.mint01`](https://raw.githubusercontent.com/disorderedmaterials/dissolve/develop/examples/emim-oac/data/SLS39568-50_50.mint01)|

Note that there are no exchangeable hydrogen atoms in this system.

The existing {{< module "NeutronSQ" >}} represents sample 1 in the table since it will default to use the natural isotopologues for the cation and anion, so all that remains to do for that one is to set its identifying name and the reference datafile:

{{< action type="mouse">}}Click on the first {{< module "NeutronSQ" >}} module to select it{{< /action >}}
{{< action type="edit" text="Double-click on its name and change it to `H-H`" />}}
{{< action type="edit" text="Find the **Reference Data** settings group, and for the **Reference** keyword select the file `SLS39546-H_H.mint01` and set the format of the data to `mint`" />}}

For the remaining datasets you need to drag a new {{< module "NeutronSQ" >}} into the layer from the module palette (click the {{< gui-button icon="palette" text="Show Avaliable Modules" >}} button if you can't already see it) and set the relevant isotopologues, short name, and datafile as listed above.

Finally, we need to set the instrumental broadening in the {{< module "SQ" >}} module first:

{{< action type="mouse">}}Click on the {{< module "SQ" >}} module to select it{{< /action >}}
{{< action type="edit" text="Set the **QBroadening** to `OmegaDependentGaussian` with a FWHM of `0.02`" />}}

* * *
{{< button pos="left" text="Previous Step" path="step4/">}}
{{< button pos="right" text="Next Step" path="step6/">}}
