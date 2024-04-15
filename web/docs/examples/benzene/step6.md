---
title: Step 6 - Set up Refinement
type: docs
---


Time to refine. We'll need two processing layers - one to calculate the g(r) and S(Q) from the simulation, and one to refine it against experimental data.

As noted in the introduction we have neutron-weighted experimental data, so we need a layer to calculate g(r) and neutron-weighted S(Q) for our three experimental datasets:

{{< action type="menu" text="Layer &#8680; Create &#8680; Correlations &#8680; RDF and Neutron S(Q)" />}}
{{< action type="mouse">}}Open the module palette for the layer by clicking the {{< gui-button icon="palette" text="Show Avaliable Modules" >}} button at the bottom left of the tab{{< /action >}}
{{< step >}}Drag two additional {{< module "NeutronSQ" >}} modules from the _Correlation Functions_ category to the module list, placing them after the existing {{< module "NeutronSQ" >}} module{{< /step >}}

We need to set the instrumental broadening in the {{< module "SQ" >}} module first:

{{< action type="mouse">}}Click on the {{< module "SQ" >}} module to select it{{< /action >}}
{{< action type="edit" text="Set the **QBroadening** to `OmegaDependentGaussian` with a FWHM of `0.02`" />}}

Now let's set up the three {{< module "NeutronSQ" >}} modules to calculate what we want:

### C<sub>6</sub>H<sub>6</sub>

{{< action type="mouse">}}Click on the first {{< module "NeutronSQ" >}} module to select it{{< /action >}}
{{< action type="edit" text="Double-click on its name and change it to `C6H6`" />}}
{{< action type="edit" text="In the **Reference Data** settings group find the **Reference** keyword, select the file `C6H6.mint01` and set the format of the data to `mint`" />}}


### C<sub>6</sub>D<sub>6</sub>

{{< action type="mouse">}}Click on the second {{< module "NeutronSQ" >}} module to select it{{< /action >}}
{{< action type="edit" text="Change its name to `C6D6`" />}}
{{< action type="mouse" text="In the **Control** section click the button for the **Isotopologue** option" />}}
{{< action type="mouse">}}Press the {{< gui-button icon="add" text="Species" >}} button to add the natural isotopologue for the first species present{{< /action >}}
{{< action type="edit" text="Change the isotopologue from `Natural` to `C6D6`" />}}
{{< action type="edit" text="Find the **Reference Data** settings group, and for the **Reference** keyword select the file `C6D6.mint01` and set the format of the data to `mint`" />}}

### 50:50 Mix

{{< action type="mouse">}}Click on the third {{< module "NeutronSQ" >}} module to display its options{{< /action >}}
{{< action type="edit" text="Change its name to `5050`" />}}
{{< action type="mouse" text="In the **Control** section click the button for the **Isotopologue** option" />}}
{{< action type="mouse">}}Press the {{< gui-button icon="add" text="Species" >}} button to add the natural isotopologue for the first species present{{< /action >}}
{{< step >}}Select either row in the table and click the {{< gui-button icon="add" text="Isotopologue" >}} button to insert the next unused isotopologue (i.e. the deuterated analogue){{< /step >}}

{{< tip text="The isotopologue mixture you've just defined has equal parts (weight = 1.0) of the `Natural` and `C6D6` benzene isotopologues. You can use whatever ratios make sense - Dissolve will always normalise the total weight back to 1.0 internally." />}}

{{< action type="edit" text="Finally, find the **Reference Data** settings group and for the **Reference** keyword select the file `5050.mint01` and set the format of the data to `mint`" />}}

All that remains is to add our EPSR refinement layer:

{{< action type="menu" text="Layer &#8680; Create &#8680; Refinement &#8680; Standard EPSR" />}}

All of the default values set there will be sufficient.


* * *
{{< button pos="left" text="Previous Step" path="step5/">}}
{{< button pos="right" text="Next Step" path="step7/">}}
