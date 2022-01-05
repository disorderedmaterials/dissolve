---
title: Step 9a - Centre-of-Geometry RDFs
type: docs
---


Let's create a workspace now, and add a plotting tool to it:

{{< action type="menu" text="Workspace &#8680; Create New..." >}}
{{< action type="edit" text="Double-click the tab's title and rename it to `Comparison`" >}}
{{< action type="mouse" text="Right-click in the empty workspace and add a _Graph_ gizmo to it" >}}

For sanity, we'll rename it to something we understand:

{{< action text="Make sure the new `Graph` window is selected." >}}
{{< action type="menu" text="Workspace &#8680; Rename 'Graph'..." >}}
{{< action type="edit" text="Set the new name as `COG Graph`" >}}


First of all, let's send the centre-of-geometry RDF from the current production run to this graph. You can right-click on any data shown in any plot in Dissolve to set style options and, in this case, copy the data to any workspace gizmo that will accept it.

{{< action type="tabs" text="Open the `CalculateAxisAngle` module in a tab if it isn't already open" >}}
{{< action type="mouse" text="Right-click on the data curve in the line plot of the centre-of-mass RDF" >}}
{{< step text="The **Copy to...** menu contains all the current workspace gizmos that can accept the data. There should be only one (`COG Graph`) so select it" >}}


Now we must load in our restart point containing the reference data, which we can do from the _Data Manager_. The current simulation data will not be overwritten, as we will append a descriptive suffix to the names of all the data stored in the file. As well as protecting the current data, this also means that the only way to visualise the additional data is through the use of workspaces.

{{< action type="menu" text="Simulation &#8680; Data Manager" >}}
{{< step text="In the _Reference Points_ section choose **Open**" >}}
{{< step text="Select the `reference.restart` file that we saved earlier" >}}
{{< step text=" A suffix to apply to the data will be suggested based on the filename, in this case 'reference', which is fine, so click _OK_" >}}
{{< step text="Click _close_ to exit the _Data Manager_ dialog" >}}

{{< tip text="Once a reference point has been added through the _Data Manager_ the filename and location are stored in the GUI state file, and the data will be reloaded the next time the state file gets read in. You can remove reference points from the _Data Manager_ if you no longer want them to be loaded in to the GUI the next time the input file is opened." >}}


Our reference data is now available, so let's add the centre-of-geometry RDF from before we refined the simulation on to the graph:

{{< action type="tabs" text="**Comparison** workspace tab" >}}
{{< action type="mouse" text="Right-click in an empty part of the `COG Graph` to bring up the context menu" >}}
{{< step text="Select _Add Data1D..._ to bring up a dialog containing all of Dissolve's available XY data" >}}
{{< action type="edit" text="In the filter box at the top-right enter \"RDF(AB)\" to show only relevant data" >}}
{{< step text="Select the data whose object tag ends with \"@reference\" (indicating that it comes from our reference point)" >}}


{{< cimage src="../compare-rdf.png" caption="Comparison of centre-of-geometry RDFs between reference (red) and refined (black) simulations" >}}

As we can see from the plot, the differences are quite subtle, but nevertheless they are there. On refinement we observe a decrease in the number of short contacts, suggesting a reduction in face-to-face stacking.

{{< button pos="left" text="Previous Step" path="step9/">}}
{{< button pos="right" text="Next Step" path="step9b/">}}
