---
title: Step 5 - Set up Potential Refinement
---


Add the standard [`EPSR`](../../userguide/modules/epsr) refinement layer to your simulation:

{{< action type="menu" text="Layer &#8680; Create... &#8680; Refinement... &#8680; Standard EPSR" >}}

Double-click the [`EPSR`](../../userguide/modules/epsr) module to open it up as a tab - it's always useful to see the various overview graphs it provides.  We'll need to set a relatively high starting value for **EReq** as we are basically dealing with point charges in a box, and so whatever empirical potential is necessary to drive the structure of the system needs to grow large enough to have some effect against the strong Coulombic forces present.

{{< action type="tabs" text="**EPSR** module tab" >}}
{{< action type="groups" text="Open the **Calculation** settings group" >}}
{{< step text="Set **EReq** to 50.0" >}}

On the _R-Factor_ tab of the [`EPSR`](../../userguide/modules/epsr) module you will see that the magnitude of the potential (top graph) increases rather quickly to 50 (within 100 steps in the plot below), but the quality of fit as measure by the r-factor (bottom graph) takes rather longer to reach a steady state.

{{< cimage src="../ereq50-ereq.png" caption="Evolution of empirical potential magnitude (max = 50.0)" >}}

{{< cimage src="../ereq50-rfactor.png" caption="Evolution of r-factor (EReq max = 50.0)" >}}

The structure factor of the system at this point looks something like this:

{{< cimage src="../ereq50-fq.png" caption="Stable structure factor at EReq = 50.0" >}}

The question is, can it be better? The answer is, of course, yes - the intensity of the first strong peak in the F(Q) at 1.47 &#8491; is a little low. The empirical potential will eventually sort this out (perhaps with an increase of **EReq** to 75 or so) but it takes time to get it exactly right.

{{< question text="What does the peak at _Q_ = 1.47 &#8491; correspond to, and why might it be difficult to reproduce?">}}

For the purposes of the example, let's move on and calculate something from the (semi-) refined structure.

{{< button pos="left" text="Previous Step" path="../step4/">}}
{{< button pos="right" text="Next Step" path="../step6/">}}
