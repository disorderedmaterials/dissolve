---
title: Step 9b - Molecule Orientation
---


For the distance vs axis angle we'll visualise the results side-by-side. Add two more graph gizmos to the existing workspace and given them suitable names.

{{< action type="tabs" text="**Comparison** workspace tab" >}}
{{< action type="mouse" text="Right-click in the empty workspace and add a _Graph_ gizmo to it" >}}
{{< action type="edit" text="Rename the new gizmo from the workspace menu and call it `Orientation (Reference)`" >}}
{{< step text="On the gizmo's toolbar, change the view style from `XY` to `Auto3D`" >}}
{{< step text="Right-click on the empty graph and select _Add Data2D_" >}}
{{< step text="Only two 2D datasets exist, both related to the [`CalculateAxisAngle`](../../userguide/modules/calculateaxisangle) module - choose the one tagged \"@reference\"" >}}

Let's get our refined simulation data:

{{< step text=" Add another _Graph_ gizmo to the workspace and rename it to `Orientation (Refined)`" >}}
{{< action type="tabs" text="Open the [`CalculateAxisAngle`](../../userguide/modules/calculateaxisangle) module in a tab if it isn't already open" >}}
{{< action type="mouse" text="Right-click on the distance/angle plot data" >}}
{{< step text="From the **Copy to...** menu select `Orientation (Refined)`." >}}


Back on the workspace tab we now have both datasets visible.


{{< button pos="left" text="Previous Step" path="../step8/">}}
{{< button pos="right" text="Back to Index" path="../">}}
