---
title: Step 6 - Set up Potential Refinement
type: docs
---


Add the standard EPSR-style refinement layer to your simulation:

{{< action type="menu" text="Layer &#8680; Create &#8680; Refinement &#8680; Standard EPSR" />}}

Select the {{< module "EPSR" >}} module if it isn't already. We'll need to set a relatively high starting value for **EReq** as we are basically dealing with point charges in a box, and so whatever empirical potential is necessary to drive the structure of the system needs to grow large enough to have some effect against the strong Coulombic forces present.

{{< action type="tabs">}}Go to the {{<gui-tab type="layer" text="Refine (EPSR)">}} tab {{</action>}}
{{< action type="edit">}}In {{< gui-button text="Options" type="options" icon="options" >}} go to the **Control** settings group and set **EReq** to `50.0`{{</action>}}
If we run the simulation a bit longer:
{{< action type="menu" text="Simulation &#8680; Run" />}}

On the _R-Factor_ tab of the {{< module "EPSR" >}} module you will see that the magnitude of the potential (top graph) increases rather quickly to 50 (within 100 steps in the plot below), but the quality of fit as measure by the r-factor (bottom graph) takes rather longer to reach a steady state.

{{< cimage src="../ereq50-ereq.png" caption="Evolution of empirical potential magnitude (max = 50.0)" >}}

{{< cimage src="../ereq50-rfactor.png" caption="Evolution of r-factor (EReq max = 50.0)" >}}

The structure factor of the system at this point looks something like this:

{{< cimage src="../ereq50-fq.png" caption="Stable structure factor at EReq = 50.0" >}}

The question is, can it be better? The answer is, of course, yes - the intensity of the first strong peak in the F(Q) at 1.47 &#8491; is a little low. The empirical potential will eventually sort this out (perhaps with an increase of **EReq** to 75 or so) but it takes time to get it exactly right.

{{< question text="What does the peak at _Q_ = 1.47 &#8491; correspond to, and why might it be difficult to reproduce?">}}

For the purposes of the example, let's move on and calculate something from the (semi-) refined structure.

First, stop the simulation:

{{< action type="key" text="Esc" />}}

* * *
{{< button pos="left" text="Previous Step" path="step5/">}}
{{< button pos="right" text="Next Step" path="step7/">}}
