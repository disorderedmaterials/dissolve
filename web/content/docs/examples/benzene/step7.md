---
title: Step 7 - Refine the Simulation
---


Restart the simulation and monitor the progress of the r-factors and eReq value in the [`EPSR`](../../userguide/modules/epsr) module.

{{< action type="tabs" text="**Refine (EPSR)** layer tab" >}}
{{< action type="mouse" text="Double-click the [`EPSR`](../../userguide/modules/epsr) module to open it in a tab and see all of the associated graphs" >}}
{{< action type="mouse" text="Click the `R-Factor` tab to view the `R-Factor` (bottom) and `EReq` (top) graphs" >}}


Once the additional energy has stabilised at 10.0, we can increase it to 20.0 to improve our fit a little more.

{{< action text="Pause the simulation" >}}
{{< action type="mouse" text="Find the [`EPSR`](../../userguide/modules/epsr) module options" >}}
{{< action type="edit" text="Change the **EReq** value to 20" >}}
{{< action text="Start the simulation" >}}


Keep running until the applied potential gets to the new limit - we don't want the empirical potential to be varying too much while we're conducting our production run, otherwise any properties we calculate may be calculated from a series of configuration that do no represent a stable structural point. Once it has, we can save another restart file if we want:

{{< action type="menu" text="Simulation &#8680; Save Restart Point..." >}}
{{< action type="edit" text="Change the filename to `refined.restart` and click _Save_" >}}


{{< button pos="left" text="Previous Step" path="../step6/">}}
{{< button pos="right" text="Next Step" path="../step8/">}}
