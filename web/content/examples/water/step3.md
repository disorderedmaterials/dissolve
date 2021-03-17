---
title: Step 3 - Set up Basic Processing
type: docs
weight: 5
---


We'll now create two processing layers to handle the evolution of the configuration and the calculation of radial distribution functions and structure factors.

### The Evolution Layer

We'll use the standard molecule evolution layer for our system:

{{< action type="menu" text="Layer &#8680; Create... &#8680; Evolution... &#8680; Standard Molecular (MC/MD)">}}


The new layer contains the following modules:

| Module | Purpose |
|--------|---------|
| [`MolShake`]({{< ref "molshake" >}}) | Performs standard Monte Carlo moves on individual molecules |
| [`MD`]({{< ref "md" >}}) | Performs a number of molecular dynamics steps, evolving the system according to Newton's equations of motion |
| [`Energy`]({{< ref "energy" >}}) | Calculates the total energy of configurations |

Monte Carlo simulation via the [`MolShake`]({{< ref "molshake" >}}) module will be the principal way our system moves and evolves, translating and rotating whole molecules but keeping the internal geometry of those molecules constant. The molecular dynamics run is critical for water (and, indeed, any molecular system) because it is this that will relax and evolve the intramolecular degrees of freedom in our molecules and give us realistic sampling of our bonds and angles. We don't need this to happen every iteration of the simulation, so the frequency of the [`MD`]({{< ref "md" >}}) module is set to 5 by default.

### g(r) and Structure Factors

We have neutron-weighted experimental data, so we need a layer to calculate RDFs and neutron S(Q):

{{< action type="menu" text="Layer &#8680; Create... &#8680; Calculate... &#8680; RDF and Neutron S(Q)">}}

First we need to set the instrumental broadening in the [`SQ`]({{< ref "sq" >}}) module:

{{< action type="mouse" text="Click on the `SQ` module (`SQ01`) to display its options">}}
{{< step text="Set the **QBroadening** to `OmegaDependentGaussian` with a FWHM of 0.02" >}}

This broadening is a known parameter of the SANDALS instrument on which the experimental data were collected.

{{< tip text=" For generality, some of Dissolve's broadening functions refer to 'omega', which should be taken to mean the reciprocal space axis (in this case, 'Q')." >}}

Next, we will set up our calculation of the weighted structure factors. Since a [`NeutronSQ`]({{< ref "neutronsq" >}}) module calculates the partial and total structure factors for a single isotopic composition, we will need to add two more since we have three reference datasets.

{{< action type="mouse" text="Show the module palette for the layer by clicking the **Show Palette** button at the top left of the tab.">}}
{{< step text="Drag a `NeutronSQ` module from the **Module Palette** on the left, placing it after the existing `NeutronSQ` module. Alternatively, double-click the `NeutronSQ` module in the **Module Palette** to append one to the current list of modules." >}}
{{< step text="Add a second `NeutronSQ` module, ensuring all three are **after** the `SQ` module." >}}

The [`NeutronSQ`]({{< ref "neutronsq" >}}) module calculates the structure factors from existing radial distribution functions - it does not calculate these itself. This is the role of the [`RDF`]({{< ref "rdf" >}}) module and, as such, it must be the first module in the list (i.e. be at the top).

Note that each of the new modules has a unique name - it is a requirement that modules within Dissolve can be uniquely identified by their name. We'll now give the modules sensible names that describe our three datasets, and set the isotopologues and reference data files.

### H<sub>2</sub>O

{{< action type="mouse" text="Click on the first `NeutronSQ` module (`NeutronSQ01`) to display its options">}}
{{< action type="edit" text="Change its name from `NeutronSQ01` to `H2O`">}}
{{< action type="groups" text="Open the **Reference Data** settings group" >}}
{{< step text="For the **Reference** keyword select the file `SLS18498-H2O.mint01` and set the format of the data to `mint`" >}}

### D<sub>2</sub>O

{{< action type="mouse" text="Click on the second `NeutronSQ` module (`NeutronSQ02`) to display its options" >}}
{{< action type="edit" text="Change its name from `NeutronSQ02` to `D2O`" >}}
{{< step text="In the **Control** section click the **Edit...** button for the **Isotopologue** option" >}}
{{< step text="Press the **Add** button to add a new isotopologue for each species present" >}}
{{< step text="Change the isotopologue from `Natural` to `Deuterated`" >}}
{{< action type="groups" text="Open the **Reference Data** settings group" >}}
{{< step text="For the **Reference** keyword select the file `SLS18502-D2O.mint01` and set the format of the data to `mint`" >}}

### HDO

The HDO sample is a little different in respect of the isotopologue specification. In order to get the 50:50 mix we will basically add two isotopologues for the water species - one H<sub>2</sub>O and one D<sub>2</sub>O. Each will have the same relative weighting of 1.0. Importantly, we must also tell the module that the `HW` atom type is exchangeable - otherwise, the [weighting of the intramolecular terms]({{< ref "neutronsq#weighting" >}}) will be incorrect as, in effect, we will simulate a mix of distinct H<sub>2</sub>O and D<sub>2</sub>O molecules, which in reality is not what was measured.

{{< tip text="As a general rule, any alcoholic or amine hydrogen is exchangeable." >}}

{{< action type="mouse" text="Click on the third `NeutronSQ` module (`NeutronSQ03`) to display its options" >}}
{{< action type="edit" text="Change its name from `NeutronSQ03` to `HDO`" >}}
{{< step text="In the **Control** section click the **Edit...** button for the **Isotopologue** option" >}}
{{< step text=" Press the **Add** button to add the natural isotopologue for the water species" >}}
{{< step text="Select the entry for the water species, or its isotopologue, and click the **Add** button again to insert another isotopologue (the next unused isotopologue will be added - in this case, the `Deuterated` one)." >}}
{{< step text="Click the button for the **Exchangeable** option" >}}
{{< step text=" Tick the `HW` atom type in the list" >}}
{{< action type="settings" text="Open the **Reference Data** settings group" >}}
{{< step text="For the **Reference** keyword select the file `SLS18502-HDO5050.mint01` and set the format of the data to `mint`" >}}


{{< button pos="left" text="Previous Step" path="step2/">}}
{{< button pos="right" text="Next Step" path="step4/">}}
