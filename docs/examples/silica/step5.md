---
title: Step 5 - Set up Potential Refinement
parent: Bulk Silica
grand_parent: Examples
---
# Bulk Silica

## 5. Refine the Structure

Add the standard [`EPSR`](../../userguide/modules/epsr) refinement layer to your simulation:

> Layer &#8680; Create... &#8680; Refinement... &#8680; Standard EPSR
{: .action .action_menu}

Double-click the [`EPSR`](../../userguide/modules/epsr) module to open it up as a tab - it's always useful to see the various overview graphs it provides.  We'll need to set a relatively high starting value for **EReq** as we are basically dealing with point charges in a box, and so whatever empirical potential is necessary to drive the structure of the system needs to grow large enough to have some effect against the strong Coulombic forces present.

> **EPSR** module tab
{: .action .action_tabs}
> Open the **Calculation** settings group
{: .action .action_groups}
> Set **EReq** to 50.0
{: .step}

On the _R-Factor_ tab of the [`EPSR`](../../userguide/modules/epsr) module you will see that the magnitude of the potential (top graph) increases rather quickly to 50 (within 100 steps in the plot below), but the quality of fit as measure by the r-factor (bottom graph) takes rather longer to reach a steady state.

![](ereq50-ereq.png)
_Evolution of empirical potential magnitude (max = 50.0)_

![](ereq50-rfactor.png)
_Evolution of r-factor (EReq max = 50.0)_

The structure factor of the system at this point looks something like this:

![](ereq50-fq.png)
_Stable structure factor at EReq = 50.0_

The question is, can it be better? Increase 

[Previous Step](step4.md){: .btn }   [Next Step](step6.md){: .btn .right}
