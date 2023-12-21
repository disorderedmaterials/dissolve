---
title: Step 5 - Assessing the Reference Potential
type: docs
weight: 7
---


With our equilibrated (or equilibrating...) system we'll now make a basic comparison between our simulated total structure factors and the reference datasets.

{{< action type="tabs" >}}Go to the {{< gui-tab text="G(r) / Neutron S(Q)" type="Layer" >}} tab {{< /action >}}
{{< action type="mouse" >}}Click on the "H2O" {{< module "NeutronSQ" >}} module and select the **Output** button{{< /action >}}

We'll first check the agreement between the experimental total structure factor (the F(Q)) and the simulated data. You'll see a row of buttons at the top of the graph giving you access to various different plotted quantities, with the default being the {{< gui-button text="Total F(Q)" >}} which should look a little like this:

{{< cimage src="../equilibrated-h2o-fq.png" caption="Calculated (black) and experimental (red) F(Q) for the equilibrated water (H<sub>2</sub>O) simulation" >}}

There are some small mismatches between the peak positions at $Q$ values of 3.1 and 5.1 &#8491;<sup>-1</sup> but overall the agreement is pretty good. The mismatch at $Q$ < 2 &#8491;<sup>-1</sup> is related to processing of the experimental data, and which we'll not try to do anything to fix in this example.

Let's take a look now at the total radial distribution functions:

{{< action type="mouse" >}}Click the {{< gui-button text="Total G(r)" >}} button on the "H2O" {{< module "NeutronSQ" >}} module's output page{{< /action >}}

{{< cimage src="../equilibrated-h2o-gr.png" caption="Simulated (black), Fourier-transformed from simulated F(Q) (green), and experimental (red) G(r) for the equilibrated water (H<sub>2</sub>O) simulation" >}}

On this graph you will have three curves which we'll discuss individually as the distinctions are important.

#### Calculated G(r) (black)

The black line represents the G(r) calculated directly from the atomic coordinates in the configuration, with broadening applied to the intramolecular terms (if requested).

#### Experimental G(r) (red)

The experimental G(r) is obtained by Fourier-transforming the reference data and as such is a subject to all the potential artefacts introduced by doing so. A window function is typically applied, and the operational range of the Fourier transform is often truncated in order to reduce ripple effects or to avoid known "bad" data at the lower Q end of the spectrum.

#### Simulated G(r) Calculated by Fourier Transform (green)

Since the experimental data is subjected to Fourier transform in order to generate a reference G(r), the most reliable comparison to make is to simulated data treated in the same way. Thus, the green line ("Via FT" in the plot) is obtained by double Fourier of the original calculated G(r) (black line), taking it to Q-space and back again.

So, any critical comparison between simulated and experimental G(r) should be made by comparing the red and green lines.


* * *
{{< button pos="left" text="Previous Step" path="step4/">}}
{{< button pos="right" text="Next Step" path="step6/">}}
