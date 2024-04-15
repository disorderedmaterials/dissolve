---
title: Step 7 - Refine the Simulation
type: docs
---


Restart the simulation and monitor the progress of the r-factors and eReq value in the {{< module "EPSR" >}} module.

{{< action type="tabs">}}Go to the {{< gui-tab type="layer" text="Refine (EPSR)">}} layer tab{{< /action >}}
{{< action type="mouse">}}Select the {{< module "EPSR" >}} module and show its {{< gui-button icon="output" text="Output" >}} tab{{< /action >}}
{{< action type="mouse">}}Click the {{<gui-button text="R Factors" >}}tab to display the errors{{</action>}}

You'll see here a graph of the individual R-factors for the three datasets, as well as the total (summed) R-factor. The current "strength" of applied potential is shown at the bottom-left of the plot (**Current EReq**). Once you've been running for a few hundred steps the ereq value should have increased to 3.0, and the fits might look something like this (on the {{<gui-button text="F(Q)" >}} tab of the {{< module "EPSR" >}} module):

{{< cimage src="../ereq3.png" caption="Comparison of reference vs simulated structure factors at ereq=3.0" >}}

The fits should be really quite good, but you might observe some "ripples" in the simulated structure factors at lower $Q$ values, as is the case in the image above. These do not arise from structure in the system, but rather are high-frequency Fourier ripples caused by truncation of the partial radial distribution functions, made worse in the present case because our system isn't that large (100 molecules). We can remove some of that effect by introducing a window function into the Fourier transform when generating our structure factors:

First, stop the simulation if it's running:
{{< action type="key" text="Esc" />}}


{{< action type="tabs">}}Go to the {{< gui-tab type="layer" text="RDF / Neutron S(Q)">}} layer tab{{< /action >}}
{{< action type="mouse">}}Select the {{< module "SQ" >}} module{{< /action >}}
{{< action type="edit">}}In the **Control** settings group change the **WindowFunction** to `Lorch0`{{< /action >}}


Start the simulation running again and after a few steps you should see much smoother structure factors:

{{< cimage src="../ereq3-lorch0.png" caption="Comparison of reference vs simulated structure factors at ereq=3.0, and with the Lorch0 window function applied during Fourier transform of g(r) to S(Q)" >}}

{{< warn text="Applying a window function in this way can have unwanted effects as well, with structural features \"smoothed out\". Look closely at the C<sub>6</sub>D<sub>6</sub> data around 1.5 < $Q$ < 2.0 and you can see that the pair of peaks has been somewhat blended together with the Lorch0 function applied." />}}

With our refinement progressed to an acceptable level (in the data shown above the total r-factor is of the order of 5.5&times;10<sup>-4</sup>), we're now in a position to save another restart point:

{{< action type="menu" text="File &#8680; Save Restart Point..." />}}
{{< action type="edit">}}Change the filename to `ereq3.restart` and click {{< gui-button text="Save">}}{{< /action >}}


* * *
{{< button pos="left" text="Previous Step" path="step6/">}}
{{< button pos="right" text="Next Step" path="step8/">}}
