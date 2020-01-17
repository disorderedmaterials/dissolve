---
title: Step 7 - Refine the Simulation
parent: Comparing Benzene Structure
grand_parent: Examples
---
# Comparing Benzene Structure

## 7. Refine the Simulation

Restart the simulation and monitor the progress of the r-factors and eReq value in the [`EPSR`](../../userguide/modules/epsr) module.

> **Refine (EPSR)** layer tab
{: .action .action_tabs}
> Double-click the [`EPSR`](../../userguide/modules/epsr) module to open it in a tab and see all of the associated graphs
{: ..action .action_mouse}
> Click the `R-Factor` tab to view the `R-Factor` (bottom) and `EReq` (top) graphs
{: ..action .action_mouse}

Once the additional energy has stabilised at 10.0, we can increase it to 20.0 to improve our fit a little more.

> Pause the simulation
{: .action}
> Find the [`EPSR`](../../userguide/modules/epsr) module options
{: .action .action_mouse}
> Change the **EReq** value to 20
{: .action .action_edit}
> Start the simulation
{: .action}

Keep running until the applied potential gets to the new limit - we don't want the empirical potential to be varying too much while we're conducting our production run. Once it has, we can save another restart file if we want:

> Simulation &#8680; Save Restart Point...
{: .action .action_menu}
> Change the filename to `refined.restart` and click _Save_
{: .action .action_edit}

[Previous Step](step6.md){: .btn }   [Next Step](step8.md){: .btn .right}
