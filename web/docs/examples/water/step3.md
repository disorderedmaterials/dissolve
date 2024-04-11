---
title: Step 3 - Set up Basic Processing
type: docs
weight: 5
---


We'll now create two processing layers to handle the evolution of the configuration and the calculation of radial distribution functions and structure factors.

### The Evolution Layer

We'll use the standard molecule evolution layer for our system:

{{< action type="menu" text="Layer &#8680; Create &#8680; Evolution &#8680; Standard Molecular (MC/MD)" />}}


The new layer contains the following modules:

| Module | Purpose |
|--------|---------|
| {{< module "MolShake" >}} | Performs standard Monte Carlo moves on individual molecules |
| {{< module "MD" >}} | Performs a number of molecular dynamics steps, evolving the system according to Newton's equations of motion |
| {{< module "Energy" >}} | Calculates the total energy of configurations |

Monte Carlo simulation via the {{< module "MolShake" >}} module will be the principal way our system moves and evolves, translating and rotating whole molecules but keeping the internal geometry of those molecules constant. The molecular dynamics run is critical for water (and, indeed, any molecular system) because it is this that will relax and evolve the intramolecular degrees of freedom in our molecules and give us realistic sampling of our bonds and angles. We don't need this to happen every iteration of the simulation, so the frequency of the {{< module "MD" >}} module is set to 5 by default.

### g(r) and Structure Factors

We have neutron-weighted experimental data, so we need a layer to calculate RDFs and neutron S(Q):

{{< action type="menu" text="Layer &#8680; Create &#8680; Correlations &#8680; RDF and Neutron S(Q)" />}}

First we need to set the instrumental broadening in the {{< module "SQ" >}} module:

{{< action type="mouse" >}}Click on the {{< module "SQ" >}} module to display its options {{< /action >}}
{{< action type="edit" text="Set the **QBroadening** to `OmegaDependentGaussian` with a FWHM of `0.02`" />}}

This broadening is a known parameter of the SANDALS instrument on which the experimental data were collected.

{{< tip text=" For generality, some of Dissolve's broadening functions refer to 'omega', which should be taken to mean the reciprocal space axis (in this case, 'Q')." />}}

Next, we will set up our calculation of the weighted structure factors. Since a {{< module "NeutronSQ" >}} module calculates the partial and total structure factors for a single isotopic composition, we will need to add two more since we have three reference datasets.

{{< action type="mouse" >}}Show the module palette for the layer by clicking {{< gui-button icon="palette" text="Show Available Modules" >}} at the very bottom of the module list {{< /action >}}
{{< action type="mouse" >}}Drag a {{< module "NeutronSQ" >}} module from the list of available modules, placing it after the existing {{< module "NeutronSQ" >}} module. Alternatively, double-click the {{< module "NeutronSQ" >}} entry in the list of available modules to append one to the current list of modules {{< /action >}}
{{< step >}}Add another {{< module "NeutronSQ" >}} module, ensuring all three are **after** the {{< module "SQ" >}} module {{< /step >}}

Note that each of the new {{< module "NeutronSQ" >}} modules has a unique name (`NeutronSQ`, `NeutronSQ01`, and `NeutronSQ02`) - it is a requirement that modules within Dissolve can be uniquely identified by their name. We'll now give the modules sensible names that describe our three datasets, and set the isotopologues and reference data files.

### H<sub>2</sub>O

{{< action type="mouse" >}}Click on the first {{< module "NeutronSQ" >}} module ("NeutronSQ") to display its options{{< /step >}}
{{< action type="edit">}}Change its name from "NeutronSQ" to "H2O"{{< /action >}}
{{< action type="edit" >}}In the **Reference Data** settings group, for the **Reference** keyword select the file "SLS18498-H2O.mint01" and set the format of the data to `mint`{{< /step >}}

### D<sub>2</sub>O

{{< action type="mouse" >}}Click on the second {{< module "NeutronSQ" >}} module ("NeutronSQ01") to display its options{{< /step >}}
{{< action type="edit" >}}Change its name from "NeutronSQ01" to "D2O"{{< /action >}}
{{< action type="groups" >}}In the **Isotopes & Normalisation** section click the button for the **Isotopologue** option - it will currently say {{< gui-button icon="options" text="&lt;Default to Natural&gt;" >}}{{< /action >}}
{{< action type="mouse" >}}Press the {{< gui-button icon="add" text="Species" >}} button to add a new isotopologue for each species present{{< /action >}}
{{< step text="Change the isotopologue from `Natural` to `Deuterated`" />}}
{{< action type="edit" >}}In the **Reference Data** settings group, for the **Reference** keyword select the file "SLS18502-D2O.mint01" and set the format of the data to `mint`{{< /step >}}

### HDO

The HDO sample is a little different in respect of the isotopologue specification. In order to get the 50:50 mix we will basically add two isotopologues for the water species - one H<sub>2</sub>O and one D<sub>2</sub>O. Each will have the same relative weighting of 1.0. Importantly, we must also tell the module that the `HW` atom type is exchangeable - otherwise, the [weighting of the intramolecular terms]({{< ref "neutronsq#weighting" >}}) will be incorrect as, in effect, we will simulate a mix of distinct H<sub>2</sub>O and D<sub>2</sub>O molecules, which in reality is not what was measured since the hydroxyl hydrogens undergo fast exchange and appear as a mixture on the timescale of the neutron measurements.

{{< tip text="As a general rule, any alcoholic or amine hydrogen is exchangeable." />}}

{{< action type="mouse" >}}Click on the third {{< module "NeutronSQ" >}} module ("NeutronSQ02") to display its options{{< /step >}}
{{< action type="edit">}}Change its name from "NeutronSQ02" to "HDO"{{< /action >}}
{{< action type="groups" >}}In the **Isotopes & Normalisation** section click the button for the **Isotopologue** option - it will currently say {{< gui-button icon="options" text="&lt;Default to Natural&gt;" >}}{{< /action >}}{{< action type="mouse" >}}Press the {{< gui-button icon="add" text="Species" >}} button to add the natural isotopologue for the water species{{< /action >}}
{{<action type="mouse">}}Select the **Water** species{{</action>}}
{{< action type="mouse" >}}Click the {{< gui-button icon="add" text="Isotopologue" >}} button to insert another isotopologue (the next unused isotopologue will be added - in this case, the `Deuterated` one) {{< /action >}}
{{< action type="groups" >}}In the **Isotopes & Normalisation** section click the button for the **Exchangeable** option - it will currently say {{< gui-button icon="options" text="&lt;None&gt;" >}}{{< /action >}}
{{< action type="check" text=" Tick the `HW` atom type in the list" />}}
{{< action type="edit" >}}In the **Reference Data** settings group, for the **Reference** keyword select the file "SLS18502-HDO5050.mint01" and set the format of the data to `mint`{{< /step >}}

* * *
{{< button pos="left" text="Previous Step" path="step2/">}}
{{< button pos="right" text="Next Step" path="step4/">}}
