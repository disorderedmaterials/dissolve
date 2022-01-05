---
title: Step 4b - Calculate g(r) and S(Q)
type: docs
---


The experimental data we'll be fitting to is neutron scattering data, so we will need to calculate neutron-weighted structure factors.

{{< action type="menu" text="Layer &#8680; Create... &#8680; Calculate... &#8680; RDF and Neutron S(Q)" >}}

The new layer contains the following modules:

| Module | Purpose |
|--------|---------|
| [`RDF`]({{< ref "rdf" >}}) | Calculates partial g(r) between every pair of atom types, and sums them into the total G(r) |
| [`SQ`]({{< ref "sq" >}}) | Calculates partial S(Q) between every pair of atom types by Fourier-transforming a source set of g(r), and sums them into the total F(Q) |
| [`NeutronSQ`]({{< ref "neutronsq" >}}) | Takes the S(Q) calculated by an [`SQ`]({{< ref "sq" >}}) module to generate the neutron-weighted partial and total structure factors |

This time we will need to set a few parameters in the [`NeutronSQ`]({{< ref "neutronsq" >}}) module, in particular informing it of the isotopic composition of our system and loading in reference data.

{{< warn text="A `NeutronSQ` module calculates S(Q) and F(Q) for a single isotopic composition. Where there are multiple isotopic samples, you need to add a `NeutronSQ` for each distinct isotopic composition." >}}


### Set up Isotopes

The [`NeutronSQ`]({{< ref "neutronsq" >}}) module will use isotopic natural abundances to calculate the neutron weights for all species unless we tell it otherwise. We'll first define the correct isotopologue for our argon species, and then tell [`NeutronSQ`]({{< ref "neutronsq" >}}) to use it. The experimental measurement was made on Ar<sup>36</sup> since its coherent scattering cross-section (24.9 fm) is considerably higher than that of the naturally-occuring mix (1.91 fm).

{{< action type="tabs" text="**Ar** species tab, **Isotopologues** section" >}}
{{< step text="Click **Add**" >}}
{{< step text=" Expand the entries for the 'NewIsotopologue' by clicking the [+] next to its name, or by clicking **Expand All**" >}}
{{< step text="Change the entry for the Ar atom type from `Natural (bc = 1.909 fm)` to `36 (bc = 24.9)`" >}}
{{< step text="For sanity's sake, you may also want to change the name of the isotopologue to something like 'Ar36' (double-click on the name, 'NewIsotopologue', to do so)" >}}

Now we'll go to our calculation layer and set the isotopologue for our [`NeutronSQ`]({< ref "neutronsq" >}}) module:

{{< action type="tabs" text="**RDF and Neutron S(Q)** layer tab" >}}
{{< action type="groups" text="Select the **NeutronSQ** module and open the **Control** settings group" >}}
{{< step text="Click the **Edit...** button for the **Isotopologue** option" >}}
{{< step text="Press the **Auto** button to populate the list with the default isotopic selection for each species" >}}
{{< step text="Change the isotopologue for the argon species from `Natural` to `Ar36`" >}}

{{< tip text="The 'Natural' isotopologue for each species is defined internally by Dissolve, and is always available. It does not appear in the list of defined isotopologues on the species tab." >}}

### Import Reference Data

The [`NeutronSQ`]({{< ref "neutronsq" >}}) module itself looks after any related experimental reference data that we might wish to compare our calculated data to, and which we'll now set up.

{{< action type="tabs" text="**RDF and Neutron S(Q)** layer tab" >}}
{{< action type="groups" text="Select the **NeutronSQ** module and open the **Reference Data** settings group" >}}
{{< step text="For the **Reference** keyword select the file `yarnell.sq`, and set the format of the data to `xy`" >}}
{{< action type="settings" text="Open the options for the file import " >}}
{{< action type="groups" text="Open the **Manipulations** group" >}}
{{< step text="We need to subtract the average level from the experimental data as it oscillates around 1.0, so set the **RemoveAverage** value to 9.0 &#8491;<sup>-1</sup>. This will instruct Dissolve to work out the average value of the data from a _Q_ of 9.0 &#8491;<sup>-1</sup>, and remove it from the data.">}}

The data, along with its Fourier transform, are now plotted in the graphs to the right, and you'll see that the data file name now appears in the button for the **Reference** keyword. The data are normalised to the average squared value of the atomic scattering, so we will instruct Dissolve to remove this normalisation.

{{< action type="tabs" text="**RDF and Neutron S(Q)** layer tab" >}}
{{< action type="groups" text="Open the **Reference Data** settings group" >}}
{{< step text="Set the **ReferenceNormalisation** to `AverageSquared`" >}}


{{< button pos="left" text="Previous Step" path="step4a/">}}
{{< button pos="right" text="Next Step" path="step5/">}}
