---
title: Step 4 - Assessing the F(Q)
type: docs
weight: 1
---

Start the simulation running:

{{< action type="key" text="Ctrl-R" />}}

After just a few tens of steps you should see that the {{< module "MD" >}} is doing what it's supposed to and giving our framework a little bit of disorder as it flexes and vibrates:

{{< cimage src="../framework-md.png" caption="Cu-BTC at 300 K after several iterations of molecular dynamics" >}}

If we take a look at the output of the {{< module "NeutronSQ" >}} we can see how our simulation compares to the experimental data:

{{< action type="tabs">}}Go to the {{< gui-tab type="layer" text="G(r) / Neutron S(Q)" >}} layer tab{{< /action >}}
{{< action type="mouse">}}Select the {{< module "NeutronSQ" >}} module and go to its {{< gui-button icon="output" text="Output">}} tab{{< /action >}}

{{< cimage src="../baseline-fq.png" caption="Comparison of simulated (black) and reference (red) total structure factors" >}}

We've zoomed in to quite a narrow $Q$ range (0 < $Q$ < 2.0 &#8491;<sup>-1</sup>) in the above graphic in order to show in detail the Bragg peaks from the sample, and you'll see that there is pretty good agreement between the simulated and experimental data in terms of positions, but the simulated data are way too sharp and need some broadening applied. The exact degree of broadening to be applied varies from sample to sample and depends on the crystallite size and uniformity, and the resolution of the instrument on which the data were measured. Without a calibrant sample - i.e. a crystalline sample of specific quality, crystallite size, and uniformity - against which to assess instrument resolution, a general broadening must be applied which accounts for all these possible factors.

We'll make an initial guess as to the right broadening here - first we'll try to get the peak widths at the lowest $Q$ values a bit better by applying a Q-independent factor:

{{< action type="tabs" >}}Go to the {{< gui-tab text="RDF / Neutron S(Q)" type="layer" >}} tab{{< /action >}}
{{< action type="mouse" >}}Click on the {{< module "SQ" >}} module to display its options{{< /action >}}
{{< action type="edit" >}}Find the **Bragg Scattering** options group and set the **BraggQBroadening** to `GaussianC2` with a _FWHM_ of `0.05` and a _FWHM(x)_ of `0.02`{{< /action >}}

Now run the simulation for twenty steps or so:

{{< action type="key" text="Ctrl-F" />}}
{{< action type="edit">}}Enter `20` and click {{< gui-button text="OK">}}{{< /action >}}

Let's go back to the {{< module "NeutronSQ" >}} module and see what the comparison looks like now:

{{< cimage src="../broadening1-fq.png" caption="First broadening of simulated structure factors" >}}

As you can see this looks much better! We can still observe that as we go to Bragg peaks at higher $Q$ values are too intense, so we'll increase the $Q$-dependent broadening as well:

{{< action type="edit">}}Set the **BraggQBroadening** of the {{< module "SQ" >}} module to `GaussianC2` with a _FWHM_ of `0.055` and a _FWHM(x)_ of `0.03`{{< /action >}}

{{< cimage src="../broadening2-fq.png" caption="Second broadening of simulated structure factors" >}}

Not looking too bad considering we haven't taken account of the NIMROD instrument's "interesting" peak shape, and are representing our Bragg peaks as simple Gaussians.

 * * *
{{< button pos="left" text="Previous Step" path="step3">}}
{{< button pos="right" text="Next Step" path="step5">}}
