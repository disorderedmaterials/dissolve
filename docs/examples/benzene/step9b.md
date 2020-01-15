---
title: Step 9b - Molecule Orientation
parent: Comparing Benzene Structure
grand_parent: Examples
---
# Comparing Benzene Structure

## 9b. Molecule Orientation

For the distance vs axis angle we'll visualise the results side-by-side. Add two more graph gizmos to the existing workspace and given them suitable names.

> **Comparison** workspace tab
{: .action .action_tabs}
> Right-click in the empty workspace and add a _Graph_ gizmo to it
{: .action .action_mouse}
> Rename the new gizmo from the workspace menu and call it `Orientation (Reference)`
{: .action .action_edit}
> On the gizmo's toolbar, change the view style from `XY` to `Auto3D`
{: .step}
> Right-click on the empty graph and select _Add Data2D_
{: .step}
> Only two 2D datasets exist, both related to the [`CalculateAxisAngle`](../../userguide/modules/calculatedaxisangle) module - choose the one tagged "@reference"
{: .step}

Let's get our refined simulation data:

> Add another _Graph_ gizmo to the workspace and rename it to `Orientation (Refined)`
{: .step}
> Open the [`CalculateAxisAngle`](../../userguide/modules/calculatedaxisangle) module in a tab if it isn't already open
{: .action .action_tabs}
> Right-click on the distance/angle plot data
{: .action .action_mouse}
> From the **Copy to...** menu select `Orientation (Refined)`.
{: .step}

Back on the workspace tab we now have both datasets visible.

[Previous Step](step8.md){: .btn }   [Back to Index](index.md){: .btn .right}
