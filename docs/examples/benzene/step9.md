---
title: Step 9 - Compare Data
parent: Comparing Benzene Structure
grand_parent: Examples
---
# Comparing Benzene Structure

## 9. Compare Data

Viewing the output of modules in their own tabs is convenient enough, but if we want to compare data from different sources (or different restart points) on the same plot we need to use a workspace. A workspace is an empty tab to which you can add various "gizmos" to do graphing and further data analysis. Note that the contents the workspaces are not written to the [main input file](/userguide/inputfile) - they are stored in the [GUI state file](/userguide/outputfiles/state.md).

Let's create a workspace now, and add a plotting tool to it:

> Workspace &#8680; Create New...
{: .action .action_menu}
> Right-click in the empty workspace and add a _Graph_ gizmo to it
{: .action .action_mouse}

First of all, let's send the centre-of-geometry RDF from the current production run to this graph. You can right-click on any data shown in any plot in Dissolve to set style options and, in this case, copy the data to a workspace graph.

> Open the [`CalculateAxisAngle`](/userguide/modules/calculatedaxisangle) module in a tab if it isn't already open
{: .action .action_tabs}
> Right-click on the line plot for the centre-of-mass RDF
{: .action .action_mouse}
> The **Copy to...** menu contains all the current workspace graphs that can accept the data. There should be only one

[Previous Step](step8.md){: .btn }   [Back to Index](index.md){: .btn .right}
