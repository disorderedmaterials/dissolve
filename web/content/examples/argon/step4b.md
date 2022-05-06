---
title: Step 4b - Calculate g(r) and S(Q)
type: docs
---

The experimental data we'll be fitting to is neutron scattering data, so we will need to calculate neutron-weighted structure factors.

{{< action type="menu" text="Layer &#8680; Create &#8680; Correlations &#8680; RDF and Neutron S(Q)" />}}

The new layer contains the following modules:

| Module | Purpose |
|--------|---------|
| {{< gui-module RDF >}} | Calculates partial g(r) between every pair of atom types, and sums them into the total G(r) |
| {{< gui-module "SQ" >}} | Calculates partial S(Q) between every pair of atom types by Fourier-transforming a source set of g(r), and sums them into the total F(Q) |
| {{< gui-module "NeutronSQ" >}} | Takes the S(Q) calculated by an {{< gui-module "SQ" >}} module to generate the neutron-weighted partial and total structure factors |

Note that the ordering of these three modules in the layer is important - calculating the neutron-weighted structure factors requires the unweighted structure factors from the {{< gui-module "SQ" >}} module, which in turn requires the radial distribution functions from the {{< gui-module "RDF" >}} module. Modules are run "top to bottom" in the list, fulfilling the requirement that the {{< gui-module "RDF" >}} module runs before the {{< gui-module "SQ" >}} module etc.

We will now need to set a few parameters in the {{< gui-module "NeutronSQ" >}} module, in particular informing it of the isotopic composition of our system and loading in reference data.

{{< warn text="A `NeutronSQ` module calculates S(Q) and F(Q) for a single isotopic composition. Where there are multiple isotopic samples, you need to add a `NeutronSQ` for each distinct isotopic composition." >}}


### Set up Isotopes

The {{< gui-module "NeutronSQ" >}} module will use isotopic natural abundances to calculate the neutron weights for all species unless we tell it otherwise. We'll first define the correct isotopologue for our argon species, and then tell {{< gui-module "NeutronSQ" >}} to use it. The experimental measurement was made using Ar<sup>36</sup> since its coherent scattering cross-section (24.9 fm) is considerably higher than that of the naturally-occuring mix (1.91 fm).

{{< action type="tabs" >}}Go to the {{< gui-tab type="Species" text="Ar" >}} species tab, **Isotopologues** section{{< /action >}}
{{< action type="mouse">}}Click {{< gui-button text="Add" >}} to create a new isotopologue definition assigning the default (natural) isotope to each atom type present in the species{{< /action >}}
{{< action type="edit" text="Change the entry for the Ar atom type from `Natural (bc = 1.909 fm)` to `36 (bc = 24.9 fm)`" />}}
{{< action type="edit" text="For sanity's sake, you may also want to double-click on the name of the isotopologue in order to change it to something more meaningful ('Ar36' perhaps)" />}}

Now we'll go to our calculation layer and set the isotopologue for our {{< gui-module "NeutronSQ" >}} module:

{{< action type="tabs" >}}Go to the {{< gui-tab type="Layer" text="RDF and Neutron S(Q)" >}} layer tab{{< /action >}}
{{< action type="groups" >}}Select the {{< gui-module "NeutronSQ" >}} module and open the **Control** settings group{{< /action >}}
{{< step >}}The **Isotopologues** keyword currently shows that all species will "Default to Natural" isotopologues{{< /step >}}
{{< action type="mouse" >}}Click the button for the **Isotopologue** keyword to open its full options{{< /action >}}
{{< step >}}Press the {{< gui-button text="Species" type="add" >}} button to populate the list with the default isotopic selection for each species{{< /step >}}
{{< step text="Change the isotopologue for the argon species from `Natural` to `Ar36` (assuming that you changed the name earlier!)"/>}}

{{< tip text="The 'Natural' isotopologue for each species is defined internally by Dissolve, and is always available. It does not appear in the list of defined isotopologues on the species tab." >}}

### Import Reference Data

The {{< gui-module "NeutronSQ" >}} module itself looks after any related experimental reference data that we might wish to compare our calculated data to, and which we'll now set up.

{{< action type="tabs" >}}Go to the {{< gui-tab type="Layer" text="RDF and Neutron S(Q)" >}} layer tab{{< /action >}}
{{< action type="groups" >}}Select the {{< gui-module "NeutronSQ" >}} module and open the **Reference Data** settings group{{< /action >}}
{{< action type="edit" >}}For the **Reference** keyword open the file `yarnell.sq`, and set the format of the data to `xy`{{< /action >}}

The data, along with its Fourier transform, can now be seen in the module's {{< gui-button text="Output" icon="general_output" >}} page. You may notice that the data have been normalised to the average squared value of the atomic scattering and oscillate around 1.0 - we will need to tell Dissolve to convert the data back to absolute units and make it oscillate around zero.

{{< action type="groups" >}}Select the {{< gui-module "NeutronSQ" >}} module and open the **Reference Data** settings group (go back to the {{< gui-button text="Options" type="options" >}} page if you need to){{< /action >}}
{{< action type="edit" text="Set the **ReferenceNormalisation** style to `AverageSquared`" />}}
{{< step text="This tells Dissolve that the data have been normalised, and allows Dissolve to remove that normalisation in order to get the data in the correct units" />}}
{{< action type="settings" text="Open the options for the file import at the extreme right of the **ReferenceData** keyword" />}}
{{< action type="groups" text="Open the **Manipulations** group" />}}
{{< action type="edit" text="Set the **RemoveAverage** value to `9.0`" />}}
{{< step text="This will instruct Dissolve to work out the average value of the data from a _Q_ of 9.0 &#8491;<sup>-1</sup> onwards, and subtract it from the data" />}}


* * *
{{< button pos="left" text="Previous Step" path="step4a/">}}
{{< button pos="right" text="Next Step" path="step5/">}}
