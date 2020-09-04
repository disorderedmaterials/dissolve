---
title: Step 5 - Assessing the Reference Potential
weight: 7
---


With our equilibrated (or equilibrating...) system we'll now make a basic comparison between our simulated total structure factors and the reference datasets.

{{< action type="tabs" text="**RDF / Neutron S(Q)** tab" >}}
{{< step text="Double-click on the `H2O` [`NeutronSQ`](../../userguide/modules/neutronsq) module to open its additional controls in a separate tab" >}}


Now we can see the comparison between the total radial distribution functions (top panel) and neutron-weighted structure factors (bottom panel) for the H<sub>2</sub>O sample. When the system is approaching equilibrium you should see something similar to this:

{{< cimage src="../equilibrated-h2o-gr.png" caption="Equilibrated water (H2O) G(r)" >}}
{{< cimage src="../equilibrated-h2o-fq.png" caption="Equilibrated water (H2O) F(Q)" >}}

At first glance the calculated and experimental F(Q) agree reasonably well, but the G(r) clearly show that we have a problem, with the peak for the O-H bond far too sharp and intense in the calculated data. This is less to do with inaccuracy of the intramolecular parameters present in the forcefield, and more to do with the apparent broadening of intramolecular features caused by the relative speed of the probe particle to that of the vibrational frequency of atoms in a chemical bond. To account for this difference we will apply an effective broadening function to the intramolecular g(r).

First, stop the simulation if it is running:

{{< action type="mouse" text="Click the **Pause** button on the left-hand side of the main window" >}}
{{< action type="menu" text="Simulation &#8680; Pause" >}}
{{< action type="key" text="Esc" >}}


Now we can safely adjust the options in the [`RDF`](../../userguide/modules/rdf) module:

{{< action type="tabs" text=" **RDF / Neutron S(Q)** tab" >}}
{{< step text="Select th [`RDF`](../../userguide/modules/rdf) module to display its additional controls" >}}
{{< action type="groups" text="Open the **Calculation** settings group" >}}
{{< step text="Set the **IntraBroadening** to `Gaussian` with a FWHM of 0.18 (the default)" >}}


Start the simulation up again and you should see quite quickly that this broadening function brings the intensity of the first peak down to a level much more in line with the reference data:

{{< cimage src="../equilibrated-h2o-broadened-gr.png" caption="Equilibrated water (H2O) G(r) with effective broadening applied to intramolecular g(r)" >}}
{{< cimage src="../equilibrated-h2o-broadened-fq.png" caption="Equilibrated water (H2O) F(Q) with effective broadening applied to intramolecular g(r)" >}}

It's important to understand the difference between the two flavours of "calculated" G(r). The first (`Calculated (Direct)`, solid black line) is the G(r) calculated directly from the configuration, and with broadening applied, and represents the raw "simulated" G(r). The second (`Calculated (via FT)`, dashed black line) is derived from Fourier transform of the calculate total F(Q) (i.e. it is the Fourier transform of the Fourier transform of the G(r)). The reason for displaying the latter is to enable a more consistent comparison between experimental and calculated G(r) - the reference data is Fourier transformed from _Q_- to _r_-space usually with a Window function such as the Lorch function, and so we also use the same window function when transforming the calculated F(Q). Note that the effects are mostly isolated to the first peak in the G(r), which has a lower intensity in the Fourier transformed version.

{{< button pos="left" text="Previous Step" path= "../step4/">}}
{{< button pos="right" text="Next Step" path= "../step6/">}}
