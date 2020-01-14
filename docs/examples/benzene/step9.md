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

For sanity, we'll rename it to something we understand:

> Make sure the new `Graph` window is selected.
{: .action}
> Workspace &#8680; Rename 'Graph'...
{: .action .action_menu}
> Set the new name as `COG Graph`
{: .action .action_edit}

First of all, let's send the centre-of-geometry RDF from the current production run to this graph. You can right-click on any data shown in any plot in Dissolve to set style options and, in this case, copy the data to any workspace gizmo that will accept it.

> Open the [`CalculateAxisAngle`](/userguide/modules/calculatedaxisangle) module in a tab if it isn't already open
{: .action .action_tabs}
> Right-click on the line plot for the centre-of-mass RDF
{: .action .action_mouse}
> The **Copy to...** menu contains all the current workspace gizmos that can accept the data. There should be only one (`COG Graph`) so select it.
{: .step}

Now we must load in our restart point containing the reference data which we can do from the _Data Manager_. The current simulation data will not be overwritten, as we will append a descriptive suffix to the names of all the data stored in the file. As well as protecting the current data, this also means that the only way to visualise the additional data is through the use of workspaces.

> Simulation &#8680; Data Manager
{: .action .action_menu}
> In the _Reference Points_ section choose **Open**
{: .step}
> Select the `reference.restart` file that we saved earlier
{: .step}
> A suffix to apply to the data will be suggested based on the filename, in this case 'reference', which is fine, so click _OK_
{: .step}

> Once a reference point has been added through the _Data Manager_ the filename and location are stored in the GUI state file, and the data will be reloaded the next time the state file gets read in. You can remove reference points from the _Data Manager_ if you no longer want that to happen.
{: .tip}

[Previous Step](step8.md){: .btn }   [Back to Index](index.md){: .btn .right}
