---
title: Step 4b - Calculate g(r) and S(Q)
type: docs
---

The experimental data we'll be fitting to is neutron scattering data, so we will need to calculate neutron-weighted structure factors.

{{< action type="menu" text="Layer &#8680; Create &#8680; Correlations &#8680; RDF and Neutron S(Q)" />}}

The new layer contains the following modules:

| Module | Purpose |
|--------|---------|
| {{< module "GR" >}} | Calculates partial g(r) between every pair of atom types, and sums them into the total G(r) |
| {{< module "SQ" >}} | Calculates partial S(Q) between every pair of atom types by Fourier-transforming a source set of g(r), and sums them into the total F(Q) |
| {{< module "NeutronSQ" >}} | Takes the S(Q) calculated by an {{< module "SQ" >}} module to generate the neutron-weighted partial and total structure factors |

Note that the ordering of these three modules in the layer is important - calculating the neutron-weighted structure factors requires the unweighted structure factors from the {{< module "SQ" >}} module, which in turn requires the radial distribution functions from the {{< module "GR" >}} module. Modules are run "top to bottom" in the list, fulfilling the requirement that the {{< module "GR" >}} module runs before the {{< module "SQ" >}} module etc.

We will now need to set a few parameters in the {{< module "NeutronSQ" >}} module, in particular informing it of the isotopic composition of our system and loading in reference data.

{{< warn >}}A {{< module "NeutronSQ" >}} module calculates S(Q) and F(Q) for a single isotopic composition. Where there are multiple isotopic samples, you need to add a {{< module "NeutronSQ" >}}for each distinct isotopic composition. {{< /warn >}}


### Set up Isotopes

The {{< module "NeutronSQ" >}} module will use isotopic natural abundances to calculate the neutron weights for all species unless we tell it otherwise. We'll first define the correct isotopologue for our argon species, and then tell {{< module "NeutronSQ" >}} to use it. The experimental measurement was made using Ar<sup>36</sup> since its coherent scattering cross-section (24.9 fm) is considerably higher than that of the naturally-occurring mix (1.91 fm).

{{< action type="tabs" >}}Go to the {{< gui-tab type="species"  text="Ar" >}} species tab{{< /action >}}
{{<action type="groups">}}Click on the {{<gui-tab text="Isotopologues" type="threeSpecies">}} section{{</action>}}
{{< action type="mouse">}}Click {{< gui-button text="Add" icon="Add" >}} to create a new isotopologue definition assigning the default (natural) isotope to each atom type present in the species{{< /action >}}
{{< action type="edit" text="Change the entry for the Ar atom type from `Natural (bc = 1.909 fm)` to `36 (bc = 24.9 fm)`" />}}
{{< action type="edit" text="For sanity's sake, you may also want to double-click on the name of the isotopologue in order to change it to something more meaningful ('Ar36' perhaps)" />}}

Now we'll go to our calculation layer and set the isotopologue for our {{< module "NeutronSQ" >}} module:

{{< action type="tabs" >}}Go to the {{< gui-tab type="layer" text="G(r) / Neutron S(Q)" >}} layer tab{{< /action >}}
{{< action type="groups" >}}Select the {{< module "NeutronSQ" >}} module and open its main {{< gui-button text="Options" icon="options" >}}{{< /action >}}
{{< step >}}The **Isotopologue** keyword currently shows that all species will "Default to Natural" isotopologues{{< /step >}}
{{< action type="mouse" >}}Click the button for the **Isotopologue** keyword to open its full options{{< /action >}}
{{< action type="mouse" >}}Press the {{< gui-button text="Species" icon="add" >}} button to populate the list with the default isotopic selection for each species{{< /action >}}
{{< action type="edit" text="Change the isotopologue for the argon species from `Natural` to `Ar36` (assuming that you changed the name earlier!)"/>}}

{{< tip text="The 'Natural' isotopologue for each species is defined internally by Dissolve, and is always available. It does not appear in the list of defined isotopologues on the species tab." />}}

### Import Reference Data

The {{< module "NeutronSQ" >}} module itself looks after any related experimental reference data that we might wish to compare our calculated data to, and which we'll now set up.

{{< action type="tabs" >}}Go to the {{< gui-tab type="layer" text="G(r) / Neutron S(Q)" >}} layer tab{{< /action >}}
{{< action type="groups" >}}Select the {{< module "NeutronSQ" >}} module and look for the **Reference Data** settings group{{< /action >}}
{{< action type="edit" >}}For the **Reference** keyword open the file `yarnell.sq`, and set the format of the data to `xy`{{< /action >}}

The data, along with its Fourier transform, can now be seen in the module's {{< gui-button text="Output" icon="output" >}} page. You may notice that the data have been normalised to the average squared value of the atomic scattering and oscillate around 1.0 - we will need to tell Dissolve to convert the data back to absolute units and make it oscillate around zero.

{{< action type="groups" >}}Select the {{< module "NeutronSQ" >}} module and find the **Reference Data** settings group (go back to the {{< gui-button text="Options" icon="options" >}} page if you need to){{< /action >}}
{{< action type="edit" text="Set the **ReferenceNormalisedTo** style to `SquareOfAverage`" />}}
{{< step text="This tells Dissolve that the data have been normalised, and allows Dissolve to remove that normalisation in order to get the data in the correct units" />}}
{{< action type="settings" text="Open the options for the file import at the extreme right of the **ReferenceData** keyword" />}}
{{< action type="edit" text="Find the **Manipulations** group and set the **RemoveAverage** value to `9.0`" />}}
{{< step text="This will instruct Dissolve to work out the average value of the data from a _Q_ of 9.0 &#8491;<sup>-1</sup> onwards, and subtract it from the data" />}}


* * *
{{< button pos="left" text="Previous Step" path="step4a/">}}
{{< button pos="right" text="Next Step" path="step5/">}}
