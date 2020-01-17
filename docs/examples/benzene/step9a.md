---
title: Step 9a - Centre-of-Geometry RDFs
parent: Comparing Benzene Structure
grand_parent: Examples
---
# Comparing Benzene Structure

## 9a. Centre-of-Geometry RDFs

Let's create a workspace now, and add a plotting tool to it:

> Workspace &#8680; Create New...
{: .action .action_menu}
> Double-click the tab's title and rename it to `Comparison`
{: .action .action_edit}
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

> Open the [`CalculateAxisAngle`](../../userguide/modules/calculatedaxisangle) module in a tab if it isn't already open
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

Our reference data is now available, so let's add the centre-of-geometry RDF from before we refined the simulation on to the graph:

> **Comparison** workspace tab
{: .action .action_tabs}
> Right-click in an empty part of the `COG Graph` to bring up the context menu
{: .action .action_mouse}
> Select _Add Data1D..._ to bring up a dialog containing all of Dissolve's available XY data
{: .step}
> In the filter box at the top-right enter "RDF(AB)" to show only relevant data
{: .action .action_edit}
> Select the data whose object tag ends with "@reference" (indicating that it comes from our reference point)
{: .step}

![](compare-rdf.png)
_Comparison of centre-of-geometry RDFs between reference (red) and refined (black) simulations_

As we can see from the plot, the differences are quite subtle, but nevertheless they are there. On refinement we observe a decrease in the number of short contacts, suggesting a reduction in face-to-face stacking.

[Previous Step](step9.md){: .btn }   [Next Step](ste9b.md){: .btn .right}
