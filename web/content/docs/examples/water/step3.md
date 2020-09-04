---
title: Step 3 - Set up Basic Processing
weight: 5
---


We'll now create two processing layers to handle the evolution of the configuration and the calculation of radial distribution functions and structure factors. 

### The Evolution Layer

We'll use the standard molecule evolution layer for our system:

{{< action type="menu" text="Layer &#8680; Create... &#8680; Evolution... &#8680; Standard Molecular (MC/MD)">}}


The new layer contains the following modules:

| Module | Purpose |
|--------|---------|
| [`MolShake`](../../userguide/modules/molshake) | Performs standard Monte Carlo moves on individual molecules |
| [`MD`](../../userguide/modules/md) | Performs a number of molecular dynamics steps, evolving the system according to Newton's equations of motion |
| [`Energy`](../../userguide/modules/energy) | Calculates the total energy of configurations |

Monte Carlo simulation via the [`MolShake`](../../userguide/modules/molshake) module will be the principal way our system moves and evolves, translating and rotating whole molecules but keeping the internal geometry of those molecules constant. The molecular dynamics run is critical for water (and, indeed, any molecular system) because it is this that will relax and evolve the intramolecular degrees of freedom in our molecules and give us realistic sampling of our bonds and angles. We don't need this to happen every iteration of the simulation, so the frequency of the [`MD`](../../userguide/modules/md) module is set to 5 by default.

### g(r) and Structure Factors

We have neutron-weighted experimental data, so we need a layer to calculate RDFs and neutron S(Q):

{{< action type="menu" text="Layer &#8680; Create... &#8680; Calculate... &#8680; RDF and Neutron S(Q)">}}

Since a [`NeutronSQ`](../../userguide/modules/neutronsq) module calculates the partial and total structure factors for a single isotopic composition, we will need to add two more since we have three reference datasets.

{{< action type="mouse" text="Show the module palette for the layer by clicking the **Show Palette** button at the top left of the tab.">}}
{{< step text="Drag a [`NeutronSQ`](../../userguide/modules/neutronsq) module from the **Module Palette** on the left, placing it after the existing [`NeutronSQ`](../../userguide/modules/neutronsq) module. Alternatively, double-click the [`NeutronSQ`](../../userguide/modules/neutronsq) module in the **Module Palette** to append one to the current list of modules." >}}
{{< step text="Add a second [`NeutronSQ`](../../userguide/modules/neutronsq) module, ensuring all three are **after** the [`RDF`](../../userguide/modules/rdf) module." >}}


The [`NeutronSQ`](../../userguide/modules/neutronsq) module calculates the structure factors from existing radial distribution functions - it does not calculate these itself. This is the role of the [`RDF`](../../userguide/modules/rdf) module and, as such, it must be the first module in the list (i.e. be at the top).

Note that each of the new modules has a unique name - it is a requirement that modules within Dissolve can be uniquely identified by their name. We'll now give the modules sensible names that describe our three datasets, and set the isotopologues, reference data files, and instrumental broadening as necessary. The latter is specific to each instrument and, for SANDALS, is described by a _Q_-dependent Gaussian with a FWHM of 0.02 &#8491;.

### H<sub>2</sub>O

{{< action type="mouse" text="Click on the first the [`NeutronSQ`](../../userguide/modules/neutronsq) module (`NeutronSQ01`) to display its options">}}
{{< action type="edit" text="Change its name from `NeutronSQ01` to `H2O`">}}
{{< action type="groups" text="Open the **Calculation** settings group">}}
{{< step text="Set the **QBroadening** to `OmegaDependentGaussian` with a FWHM of 0.02" >}}
{{< action type="groups" text="Open the **Neutron Isotopes** settings group">}}
{{< step text=" Click the **Edit...** button for the **Isotopologue** option">}}
{{< step text="Press the **Auto** button to populate the list with the default isotopic selection for each species" >}}
{{< action type="groups" text="Open the **Reference Data** settings group" >}}
{{< step text="For the **Reference** keyword select the file `SLS18498-H2O.mint01` and set the format of the data to `mint`" >}}

{{< tip text=" For generality, some of Dissolve's broadening functions refer to 'omega', which should be taken to mean the reciprocal space axis (in this case, 'Q')." >}}

### D<sub>2</sub>O

{{< action type="mouse" text="Click on the second the [`NeutronSQ`](../../userguide/modules/neutronsq) module (`NeutronSQ02`) to display its options" >}}
{{< action type="edit" text="Change its name from `NeutronSQ02` to `D2O`" >}}
{{< action type="groups" text="Open the **Calculation** settings group" >}}
{{< step text="Set the **QBroadening** to `OmegaDependentGaussian` with a FWHM of 0.02" >}}
{{< action type="groups" text="Open the **Neutron Isotopes** settings group" >}}

{{< step text="Click the **Edit...** button for the **Isotopologue** option" >}}
{{< step text="Press the **Auto** button to add the natural isotopologue for each species present" >}}
{{< step text="Change the isotopologue from `Natural` to `Deuterated`" >}}
{{< action type="groups" text="Open the **Reference Data** settings group" >}}
{{< step text="For the **Reference** keyword select the file `SLS18502-D2O.mint01` and set the format of the data to `mint`" >}}


### HDO

The HDO sample is a little different in respect of the isotopologue specification. In order to get the 50:50 mix we will basically add two isotopologues for the water species - one H<sub>2</sub>O and one D<sub>2</sub>O. Each will have the same relative weighting of 1.0. Importantly, we must also tell the module that the `HW` atom type is exchangeable - otherwise, the weighting of the intramolecular terms will be incorrect.

{{< tip text="As a general rule, any alcoholic or amine hydrogen is exchangeable." >}}

When we set the isotopologues for our water molecule in the `HDO` sample they were added with equal weight (1.0 in both cases) which will result in the weighted structure factors _for that species_ reflecting a 50:50 mixture. Furthermore, we had to specify that the `HW` atom type is exchangeable. When a mixture of isotopologues that contain exchangeable atoms is present, if we do not tell Dissolve that this is the case the calculate intramolecular structure factors will be wrongly weighted as, in effect, we will simulate a mix of distinct H<sub>2</sub>O and D<sub>2</sub>O molecules, which in reality is not what was measured.

{{< action type="mouse" text="Click on the third the [`NeutronSQ`](../../userguide/modules/neutronsq) module (`NeutronSQ03`) to display its options" >}}
{{< action type="edit" text="Change its name from `NeutronSQ03` to `HDO`" >}}
{{< action type="settings" text="Open the **Calculation** settings group" >}}
{{< step text="Set the **QBroadening** to `OmegaDependentGaussian` with a FWHM of 0.02" >}}
{{< action type="settings" text="Open the **Neutron Isotopes** settings group" >}}

{{< step text=" Click the **Edit...** button for the **Isotopologue** option" >}}
{{< step text=" Press the **Auto** button to add the natural isotopologue for each species present" >}}
{{< step text="Select the entry for the water species, or its isotopologue, and click the **Add** button to insert another isotopologue (the next unused isotopologue will be added - in this case, the `Deuterated` one)." >}}
{{< step text="Click the button for the **Exchangeable** option" >}}
{{< step text=" Tick the `HW` atom type in the list" >}}
{{< action type="settings" text="Open the **Reference Data** settings group" >}}
{{< step text="For the **Reference** keyword select the file `SLS18502-HDO5050.mint01` and set the format of the data to `mint`" >}}


{{< button pos="left" text="Previous Step" path= "../step2/">}}
{{< button pos="right" text="Next Step" path= "../step4/">}}
