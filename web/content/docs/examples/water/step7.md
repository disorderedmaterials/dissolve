---
title: Step 7 - Set up Potential Refinement
weight: 9
---


Let's briefly recap what we've done so far:

1. Set up a liquid water system based on a literature forcefield (`SPC/Fw`, see [http://dx.doi.org/10.1063/1.2136877](http://dx.doi.org/10.1063/1.2136877))
2. Equilibrated the system and made an initial structural comparison with experimental data
2. Adjusted the intramolecular geometry of the water molecule in order to better match the experimental data

Our agreement with experiment is OK, but it is possible to make it even better by modifying the _inter_-atomic interaction parameters contained in the atom types. However, generally this is not to be attempted by hand in all but the simplest of cases, as the effects of adjusting the interatomic are seldom as obvious as those for _intra_-molecular parameters. Also, for even a modestly-complicated system the number of adjustable parameters is simply too large to tackle with manual fitting.

Here we'll employ the [`EPSR`](../../userguide/modules/epsr) module in order to adjust the interatomic potentials automatically to give better agreement with the experimental reference data.

{{< action type="menu" text="Layer &#8680; Create... &#8680; Refinement... &#8680; Standard EPSR" >}}

Our new layer contains only the [`EPSR`](../../userguide/modules/epsr) module, and which Dissolve has set up with sensible targets and defaults. Double-click the [`EPSR`](../../userguide/modules/epsr) module to open it up as a tab - we'll need to explore the various graphs as we proceed, but for now let's check the set-up of the module. Brief descriptions of the important parameters are given below - for more in-depth explanations see the [`EPSR`](../../userguide/modules/epsr) module page.

{{< action type="tabs" text="**Refine (EPSR)** tab" >}}
{{< action type="mouse" text="Double-click on the [`EPSR`](../../userguide/modules/epsr) module to open it up in a new tab" >}}
{{< action type="groups" text=" Open the **Calculation** settings group" >}}
{{< step text=" An initial value for **EReq** has been set (3.0) - this determines the magnitude or \"strength\" of the generated interatomic potentials" >}}
{{< step text="The **Feedback** factor is 0.8 - this states that we are 80% confident in the experimental data, and that the calculated partial structure factors should make up 20% of the estimated partials" >}}
{{< step text="The range of data over which to generate the potential in _Q_-space is determined by **QMax** (30 &#8491;<sup>-1</sup>)and **QMin** (0.5 &#8491;<sup>-1</sup>)" >}}
{{< step text=" The experimental data to use in the refinement are set in the **Target** option, which lists all available modules by name that have suitable data for the [`EPSR`](../../userguide/modules/epsr) module to use. All of the [`NeutronSQ`](../../userguide/modules/neutronsq) modules have been selected automatically" >}}


All of these default values are fine for our purpose, and there's very little that you should have to change in the first instance. So, start the simulation running again to begin the refinement process, and while it's running we'll go through the different tabs in the [`EPSR`](../../userguide/modules/epsr) module one by one to see what information we have available, and which help to illustrate the basic workflow of the EPSR methodology.

### 1. F(Q)

Basic comparison between reference experimental data (solid lines) defined in [`NeutronSQ`](../../userguide/modules/neutronsq) module **Target**s and those simulated by Dissolve (dashed lines), including the difference functions (dotted lines).

{{< cimage src="../epsrmodule-fq.png" caption="Experimental, calculated, and difference functions for data targeted by the EPSR module" >}}

### 2. F(Q) Delta/Fit

Delta functions between experiment and simulation are repeated here in negated form, and shown along with the fits to the functions achieved by the Poisson / Gaussian sums.

{{< cimage src="../epsrmodule-fqdelta.png" caption="Delta functions (negated) and the Poisson/Gaussian fits to those functions" >}}

### 3. Estimated S(Q)

Estimated (**not** experimental) partial structure factors (solid lines) derived from combining experimental F(Q) and the calculated partial S(Q) (shown as dashed lines).

{{< cimage src="../epsrmodule-sq.png" caption="Estimated partial structure factors" >}}

### 4. Estimated g(r)

Partial radial distribution functions for each atom type pair in the system, showing estimated partials (solid lines, calculate by Fourier transform of the estimated S(Q)) and those from the simulation (dashed lines, calculated by Fourier transform of the simulated partial g(r)).

Note that the estimated g(r) from the combination of the experimental and simulated data are _intermolecular_ partials - i.e. they contain no contributions from _intramolecular_ terms related to the intramolecular bonding. The simulated partial g(r) (dashed lines), however, are the those calculated directly from the configuration(s) in the simulation, and include both intermolecular and intramolecular terms. The reason for this is to aid in detection of cases where the intramolecular structure of the molecules in the simulation is significantly different from that measured experimentally, which would manifest as small features in the estimated g(r) at or around the _r_-values associated to bond or angle (1,3) distances.

{{< cimage src="../epsrmodule-gr.png" caption="Estimated partial radial distribution functions" >}}

### 5. Total G(r)

Neutron weighted total G(r) from Fourier transform of the reference data (solid line) and Fourier transform of the simulated dat (dashed line). The reason for displaying the latter rather than the G(r) calculated directly in _r_-space is to provide for a consistent comparison between simulation and experiment given the necessity of performing Fourier transforms (truncation errors are worse in _Q_-space data, so transforming to _Q_ to _r_ demands more judicious use of window functions in the transform).

{{< cimage src="../epsrmodule-gr.png" caption="Total radial distribution functions from Fourier transform of reference and simulated F(Q) data" >}}

### 6. &phi;(r)

&phi;(r) represent the empirical potentials generated from the difference data, one per pair potential.

{{< cimage src="../epsrmodule-phir.png" caption="Empirical potentials for each atom type pair (i.e. each distinct pair potential)" >}}

### 7. R-factor

The upper panel shows the evolution of the current potential magnitude (whose limit is guided by the value of **EReq**). In the lower panel, for each experimental dataset the "goodness of agreement" with simulation is represented by the r-factor, and the total (average) r-factor across all datasets.

{{< cimage src="../epsrmodule-rfactor.png" caption="R-factors for experimental (reference) datasets" >}}

### 8. Debug

Safe to say that you can probably ignore this one!


{{< button pos="left" text="Previous Step" path= "../step6/">}}
{{< button pos="right" text="Next Step" path= "../step8/">}}
