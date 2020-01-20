---
title: Comparing Benzene Structure
parent: Examples
has_children: true
nav_order: 4
---
# Comparing Benzene Structure

60 - 75 minutes
{: .label .label-blue }

## Summary

You might always be asking the question "How much does quantity _X_ differ between the equilibrated simulation at the reference forcefield parameters, and the refined simulation at the empirical parameters?". This is a very fair question, and this example will show how to checkpoint simulation data for comparison later on using some custom workspace graphs. It will also introduce the utility of size factor scaling when dealing with molecular systems containing rings.

You will set up and equilibrate a small liquid benzene simulation before calculating a few properties of interest, and then refine the potential against experimental data and recalculate the properties of interest before graphing both side-by-side. The data are three neutron scattering datasets measured on the NIMROD diffractometer at the ISIS Pulsed Neutron and Muon Source in 2014.

## Preparation

Download the example data file archive `Dissolve-X.Y.Z-ExampleData.zip` or `Dissolve-X.Y.Z-ExampleData.tar.gz` (where `X.Y.Z` will be the current version) from the [continuous build](https://github.com/trisyoungs/dissolve/releases/tag/continuous) in the releases section on GitHub if you haven't done so already.

> Open the Dissolve GUI
{: .action .action_dissolve}
> Choose **Empty Simulation** from the _Create_ section
{: .step}
> Save As...
{: .action .action_menu}
> Save your own input file under a sensible name in the `benzene` directory of the example data
{: .step}

Alternatively, download the data files listed below and save them to your location of choice.

- C<sub>6</sub>H<sub>6</sub>: [C6H6.mint01](https://raw.githubusercontent.com/trisyoungs/dissolve/develop/examples/benzene/data/C6H6.mint01)
- C<sub>6</sub>H<sub>6</sub>:C<sub>6</sub>D<sub>6</sub> (50:50): [5050.mint01](https://raw.githubusercontent.com/trisyoungs/dissolve/develop/examples/benzene/data/5050.mint01)
- C<sub>6</sub>D<sub>6</sub>: [C6D6.mint01](https://raw.githubusercontent.com/trisyoungs/dissolve/develop/examples/benzene/data/C6D6.mint01)

[Let's begin!](step1.md){: .btn }
## 1. Create the Species

### Draw the Molecule

The first thing we'll do is draw a benzene molecule as best we can:

> Species &#8680; Create... &#8680; Draw_
{: .action .action_menu}

A new editor window opens in which we can draw the rough geometry and connectivity for our molecule.

> Choose "Draw mode" from the toolbar above the species viewer - ![Draw mode](../icons/viewer_edit.png)
{: .action .action_mouse}
> Draw a roughly hexagonal ring of six carbon atoms (carbon is the default drawing element) by left-click-dragging in the viewer
{: .step}
> Change the drawing element from `C` to `H` by clicking on the button next to ![Draw mode](../icons/viewer_edit.png)
{: .step}
> Connect a single hydrogen atom to each carbon by left-click-dragging from each carbon site 
{: .step}
> Left-click somewhere in the viewer outside the ring to place a hydrogen atom
{: .step}
> Click **OK** to close the editor and create the new species
{: .step}
> Double-click on the new species' tab and change its name to `Benzene`
{: .action .action_edit}

### Apply a Forcefield

Time to make it a little prettier!  We'll assign a standard forcefield to it, and optimise the geometry:

> Species &#8680; Add Forcefield Terms
{: .action .action_menu}
> From the available forcefields choose `OPLSAA2005/Aromatics` and click _Next_{: .text-green-100}
{: .step}
> For the atom types we want to _Assign all atom types, overwriting any that already exist_{: .text-green-100}. For the intramolecular terms we want to assign them and reduce to master terms. Leave the settings as they currently are, and click _Next_{: .text-green-100}
{: step}
> There should be no conflicts with existing atom types or master terms, so click _Next_{: .text-green-100} then _Finish_{: .text-green-100} to exit the wizard.
{: .step}
> Click the "medic" icon in the species viewer toolbar - ![Medic - Geometry optimise the current species](../icons/viewer_medic.png)
{: .action .action_mouse}

We will also get ahead here and edit the master terms to reflect the geometry observed in the experimental data, since the forcefield we've applied here doesn't get things quite right.

> **Forcefield** tab, **Master Terms** section
{: .action .action_tabs}
> Change the bond length (_Parameter 2_) of the `CA-HA` bond term from 1.08 to 1.09.
{: .step}
> Change the bond length (_Parameter 2_) of the `CA-CA` bond term from 1.40 to 1.38.
{: .step}

### Create Isotopologues

Since some of the experimental data was measured on deuterated benzene, we'll need to create a suitable C<sub>6</sub>D<sub>6</sub> isotopologue:

> **Benzene** species tab, **Isotopologues** section
{: .action .action_tabs}
> Click _Add_{: .text-green-100}
{: .step}
> Expand the entries for the 'NewIsotopologue' by clicking the [+] next to its name, or by clicking _Expand All_{: .text-green-100}
{: .step}
> Change the isotope for the HA atom type from `Natural (bc = -3.739 fm)` to `2 (bc = 6.671)` by clicking on the isotopologue entry and chosing from the drop-down menu
{: .step}
> Change the name of the isotopologue to 'C6D6' (click on the name to do so)
{: .step}

The other isotopologues we need are either generated automatically by Dissolve (i.e. the natural isotopologue for C<sub>6</sub>H<sub>6</sub>) or are created by mixing existing isotopologues (as will be the case for the 50:50 mix).

### Add Analysis Sites

We'll locate our analysis site at the centre of the benzene ring and give it some axes so that we may calculate orientational / spatial functions around it. The figure below shows the atoms we'll select to define the origin (purple), x-axis (red) and y-axis (blue). Using these atoms as refence points for our corrodinate system will set the XY plane to that of the ring, with the _z_ axis perpendicular to the ring.

![](site.png){: .img-centre}
*Origin (purple), x-axis (red) and y-axis (blue) atoms defining the oriented benzene site*

> **Benzene** species tab, **Analysis Sites** section
{: .action .action_tabs}
> Select all six carbon atoms in the viewer (you may need to click _reset view_ first, to see the whole molecule)
{: .action .action_mouse}
> Click **Create** in the viewer toolbar to create a new site with the origin at the center between these six atoms
{: .step}
> Now select the single carbon atom and click **X Axis** in the toolbar to define the direction of the _x_ axis
{: .step}
> Finally, select the pair of adjacent carbon atoms and click **Y Axis** in the toolbar to define the direction of the _y_ axis
{: .step}
> Rename the site to `COG` by double-clicking `C6`
{: .action action_edit}

[Back to Overview](index.md){: .btn }   [Next Step](step2.md){: .btn .right}
## 2. Set up a Configuration

Time to create a suitable liquid configuration for our simulation using the predefined "simple random mix" generator:

> Configuration &#8680; Create... &#8680; Simple random mix
{: .action .action_menu}
> Choose the benzene species and press **Select**
{: .step}
> Double-click on the tab title and change its name to `Liquid`
{: .action .action_edit}

In the interests of speed, we'll use the default number of molecules (100) for the simulation, but we will need to set the correct density. We'll use chemical units of g/cm<sup>3</sup> to specify it:

> Open the settings for the `Parameters` node
{: .action .action_settings}
> Check that the value of the "rho" variable is 0.876. Note that this is "just a number" - units are given in the `AddSpecies` node when we reference this value.
{: .step}
> Open the settings for the `AddSpecies` node
{: .action .action_settings}
> Make sure that the units for the **Density** are set to `g/cm3` and that it references our parameter `rho`.
{: .step}
> Click _Regenerate_{: .text-green-100} to recreate the configuration with our adjusted parameters.
{: .action .action_mouse}

The resulting box will have a side length of just over 24.5 &#8491;.

![](benzenebox.png){: .img-centre}
*Initial, randomised benzene box containing 100 molecules*

Since the molecules were added to the box completely at random, there will most likely be some that overlap quite a lot. Since our molecule has a ring, this can lead to situations where molecules become interlocked, and which will persist throughout the simulation unless we deal with them. The easiest thing to do is to to what Dissolve (and EPSR) call "size factor scaling". Basically, this allows the configuration box and the positions of the molecules to be scaled temporarily, removing bad overlaps and letting molecules move safely apart. The factor by which the box is scaled is reduced gradually until a value of 1.0 (i.e. the initial system size) is regained.

In anticipation of some bad contacts:

> **Liquid** configuration tab
{: .action .action_tabs}
> Set the **Requested** value for the _Size Factor Scaling_ to 10.0.
{: .step}

Finally, before we move on, we will have to reduce our pair potential range from the default of 15 &#8491; since this is larger than the half the maximum width of our simulation box.

> **Forcefield** tab, **Pair Potentials** section
{: .action .action_tabs}
> Reduce the pair potential **Range** from 15 to 12 &#8491;
{: .step}

[Previous Step](step1.md){: .btn }   [Next Step](step3.md){: .btn .right}
## 3. Set up Equilibration

To equilibrate our system we'll now add a standard Monte Carlo / Molecular Dynamics processing layer:

> Layer &#8680; Create... &#8680; Evolution... &#8680; Standard Molecular (MC/MD)
{: .action .action_menu}

We can now start running our simulation and start to equilibrate the configuration box.

> Click the _Run_{: .text-green-100} button on the left-hand side of the main window
{: .action .action_mouse}
> Simulation &#8680; Run
{: .action .action_menu}
> Ctrl-R
{: .action .action_key}

If you're watching the box in the configuration tab you'll notice that it intially "explodes" because of our defined size factor of 10.0, but the box will quickly reduce in size back to normal and result in a condensed phase with no interlocked benzene rings.

When the size factor has reduced the inter-molecular energy of the simulation will fluctuate a little, but should oscillate around some equilibrium value, at which point you can proceed. You can check this from the graphs of the [`Energy`](../../userguide/modules/energy) module:

> **Evolve (Standard)** layer tab
{: .action .action_tabs}
> Double-click the [`Energy`](../../userguide/modules/energy) module to open it in a separate tab
{: ..action .action_mouse}

Once this has been achieved, we can make a snapshot of the current simulation and store this equilibrated point as a backup. The restart file written by Dissolve as it runs stores all the information necessary to continue a stopped simulation, but there is no going back to a previous point in the simulation (e.g. to undo a bad analysis choice, or to reference earlier data) unless we have a suitable restart point to go back to. At any time we can write a new restart file from the GUI, independent of the standard restart file, and which we can keep to load in at a later date.

> Simulation &#8680; Save Restart Point...
{: .action .action_menu}
> Change the filename to `equilibrated.restart` and click _Save_
{: .action .action_edit}

Now we can move on to set up the analysis of the structural properties we're interested in.

[Previous Step](step2.md){: .btn }   [Next Step](step4.md){: .btn .right}
## 4. Set up Analysis

For our "properties of interest" we'll calculate both the centre-of-geometry RDF and the out-of-plane angle between molecules as a function of distance. We can get both from the [CalculateAxisAngle](../../userguide/modules/calculatedaxisangle) module:

| Module | Purpose |
|--------|---------|
| [`CalculateAxisAngle`](../../userguide/modules/calculatedaxisangle) | Calculates the distance vs. axis angle map between two sites |

Since our site (`COG`) is set up for the Z axis to be pointing out of the plane of the benzene ring, it is the angle formed between these axes on the different molecules that we'll consider.

Let's add an empty processing layer to to the simulation, add on our module, and set it up:

> Layer &#8680; Create... &#8680; Empty
{: .action .action_menu}
> Double-click the layer tab and rename it to `Analysis`
{: .step}
> Show the module palette for the current layer by clicking the _Show Palette_{: .text-green-100} button at the top left of the tab
{: .action .action_mouse}
> Drag a [`CalculateAxisAngle`](../../userguide/modules/calculatedaxisangle) module over to the **Current Modules** list
{: .action .action_mouse}
> Select the [`CalculateAxisAngle`](../../userguide/modules/calculatedaxisangle) module to show its options
{: .step}
> Open the **Calculation** settings group
{: .action .action_groups}
> Leave the **DistanceRange** settings as-is, but change the Max and Bin Width for the **AngleRange** to 90 and 10 respectively
{: .action .action_edit}
> Open the **Sites** settings group
{: .action .action_groups}
> Set both **SiteA** and **SiteB** to `COG`
{: .step}
> Set the **AxisA** and **AxisB** to `ZAxis`
{: .step}
> Enable the **ExcludeSameMolecule** to prevent the unwanted self-correlation spike at 0 in the resulting RDF
{: .step}

> It would also be nice here to calculate the spatial density function and compare it, but this takes more iterations than a (sane) example will allow.
{: .tip}

[Previous Step](step3.md){: .btn }   [Next Step](step5.md){: .btn .right}
## 5. Calculate Baseline Properties

Now that the analysis processing layer is prepared, we need to run it for a while in order to get some good statistics on our quantities. A good 500 iterations  (preferably 1000) is needed to get the statistics for the 2D map, so let's ask Dissolve to run for a set number of steps:

> Simulation &#8680; Run For...
{: .action .action_menu}
> Enter 1000 and click _OK_
{: .action .action_edit}

Dissolve will now iterate for 1000 iterations and stop - you can see the estimated time (ETA) to complete these 1000 steps in the status bar at the bottom of the main window. Now might be a good time to have a drink, or write that email you've been putting off. If you think 1000 iterations is going to take too long, press **Escape** to stop the simulation and go for 500 iterations instead.

To see what's being calculated, open up the [`CalculateAxisAngle`](../../userguide/modules/calculatedaxisangle) in a separate tab:

> **Analysis** layer tab
{: .action .action_tabs}
> Double-click the [`CalculateAxisAngle`](../../userguide/modules/calculatedaxisangle) module to open it in a separate tab
{: ..action .action_mouse}

We have three graphs - top-left is the radial distribution function between the centres-of-geometry of the benzene, while the bottom-left is the angle histogram of the z-axis angles (averaged over all distances). The main graph to the right shows the distance-angle map of the two quantities. You might want to explore the latter in 3D space, rather than a top-down view - you can change the view style for the plot in the toolbar at the top (change the view type from `XY` to `3D`).

Once the iterations have completed, we need to do a bit of housekeeping.

### Snapshot the Current Data

Let's save the current data now, as it represents our reference point to which we'll compare later on:

> Simulation &#8680; Save Restart Point...
{: .action .action_menu}
> Change the filename to `reference.restart` and click _Save_
{: .action .action_edit}

### Clear the Current Data

Since we're about to refine our simulation against experimental data and re-run the analysis, we don't want to retain any of the currently-calculated quantities. We can request that Dissolve clear all of the data generated by modules and reset the simulation back to an "empty" state. Critically, the current contents of any configurations will remain untouched (i.e. our equilibrated box will remain as-is, so we can proceed directly on with the simulation). It will also reset the iteration counter to zero.

Time to get rid of that data...

> Simulation &#8680; Clear Module Data
{: .action .action_menu}
> Accept the consequences, and click _OK_
{: .step}

### Turn off the Analysis Layer

We need to refine the simulation before we calculate our properties again, so we need to temporarily turn off our analysis layer:

> **Analysis** layer tab
{: .action .action_tabs}
> Disable the layer by clicking the ![](../icons/slider-on.png) button so it displays ![](../icons/slider-off.png)
{: .step}

[Previous Step](step4.md){: .btn }   [Next Step](step6.md){: .btn .right}
## 6. Set up Refinement

Time to refine. We'll need two processing layers - one to calculate the g(r) and S(Q) from the simulation, and one to refine it against experimental data.

As noted in the introduction we have neutron-weighted experimental data, so we need a layer to calculate g(r) and neutron-weighted S(Q) for our three experimental datasets:

> Layer &#8680; Create... &#8680; Calculate... &#8680; RDF and Neutron S(Q)
{: .action .action_menu}
> Show the module palette for the layer by clicking the _Show Palette_{: .text-green-100} button at the top left of the tab.
{: .action .action_mouse}
> Drag two additional [`NeutronSQ`](../../userguide/modules/neutronsq) modules from the `Correlation Functions` in the _Module Palette_{: .text-green-100} on the left, placing them after the existing [`RDF`](../../userguide/modules/rdf) module
{: .step}

Let's set up the three [`NeutronSQ`](../../userguide/modules/neutronsq) modules to calculate what we want:

### C<sub>6</sub>H<sub>6</sub>

> Click on the first the [`NeutronSQ`](../../userguide/modules/neutronsq) module to display its options
{: .action .action_mouse}
> Change its name to `C6H6`
{: .action .action_edit}
> Open the **Calculation** settings group
{: .action .action_groups}
> Set the **QBroadening** to `OmegaDependentGaussian` with a FWHM of 0.02
{: .step}
> Open the **Neutron Isotopes** settings group
{: .action .action_groups}
> Click the _Edit..._{: .text-green-100} button for the **Isotopologue** option
{: .step}
> Press the _Auto_{: .text-green-100} button to populate the list with the default isotopic selection for each species
{: .step}
> Open the **Reference Data** settings group
{: .action .action_groups}
> For the **Reference** keyword select the file `C6H6.mint01` and set the format of the data to `mint` instead of `xy`
{: .step}

### C<sub>6</sub>D<sub>6</sub>

> Click on the second the [`NeutronSQ`](../../userguide/modules/neutronsq) module to display its options
{: .action .action_mouse}
> Change its name to `C6D6`
{: .action .action_edit}
> Open the **Calculation** settings group
{: .action .action_groups}
> Set the **QBroadening** to `OmegaDependentGaussian` with a FWHM of 0.02
{: .step}
> Open the **Neutron Isotopes** settings group
{: .action .action_groups}
> Click the _Edit..._{: .text-green-100} button for the **Isotopologue** option
{: .step}
> Press the _Auto_{: .text-green-100} button to add the natural isotopologue for each species present
{: .step}
> Change the isotopologue from `Natural` to `Deuterated`
{: .step}
> Open the **Reference Data** settings group
{: .action .action_groups}
> For the **Reference** keyword select the file `C6D6.mint01` and set the format of the data to `mint`
{: .step}

### 50:50 Mix

> Click on the third the [`NeutronSQ`](../../userguide/modules/neutronsq) module to display its options
{: .action .action_mouse}
> Change its name to `5050`
{: .action .action_edit}
> Open the **Calculation** settings group
{: .action .action_settings}
> Set the **QBroadening** to `OmegaDependentGaussian` with a FWHM of 0.02
{: .step}
> Open the **Neutron Isotopes** settings group
{: .action .action_settings}
> Click the _Edit..._{: .text-green-100} button for the **Isotopologue** option
{: .step}
> Press the _Auto_{: .text-green-100} button to add the natural isotopologue for each species present
{: .step}
> Select the entry for the benzene species, or its isotopologue, and click the _Add_{: .text-green-100} button to insert the next "unused" isotopologue (i.e. the deuterated analogue).
{: .step}
> Open the **Reference Data** settings group
{: .action .action_settings}
> For the **Reference** keyword select the file `5050.mint01` and set the format of the data to `mint`
{: .step}

All that remains is to add our EPSR refinement layer:

> Layer &#8680; Create... &#8680; Refinement... &#8680; Standard EPSR
{: .action .action_menu}
> Select the [`EPSR`](../../userguide/modules/epsr) module to display its options
{: .step}
> Change the **EReq** value to 10.0
{: .action .action_edit}

[Previous Step](step5.md){: .btn }   [Next Step](step7.md){: .btn .right}
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
## 8. Calculate Refined Properties

We now have our refined simulation that is in better agreement with the experimental data, so its time to recalculate our properties so that we can compare them to those we saved earlier. All the processing layers should now be enabled and remain on for the production run, including the refinement layer.

> **Analysis** layer tab
{: .action .action_tabs}
> Enable the layer by clicking the ![](../icons/slider-off.png) button so it displays ![](../icons/slider-on.png)
{: .step}
> Simulation &#8680; Run For...
{: .action .action_menu}
> Enter 1000 and click _OK_
{: .action .action_edit}

If you're ready for another drink, now would be another opportune moment, or perhaps you could buy that thing you've been meaning to get from your favourite online retailer (no endorsements here!).

Once those iterations have completed, we can move on to do the comparison of the data. Also, we can snapshot our production run data:

> Simulation &#8680; Save Restart Point...
{: .action .action_menu}
> Change the filename to `production.restart` and click _Save_
{: .action .action_edit}

[Previous Step](step7.md){: .btn }   [Next Step](step9.md){: .btn .right}
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
> Right-click on the data curve in the line plot of the centre-of-mass RDF
{: .action .action_mouse}
> The **Copy to...** menu contains all the current workspace gizmos that can accept the data. There should be only one (`COG Graph`) so select it.
{: .step}

Now we must load in our restart point containing the reference data, which we can do from the _Data Manager_. The current simulation data will not be overwritten, as we will append a descriptive suffix to the names of all the data stored in the file. As well as protecting the current data, this also means that the only way to visualise the additional data is through the use of workspaces.

> Simulation &#8680; Data Manager
{: .action .action_menu}
> In the _Reference Points_ section choose **Open**
{: .step}
> Select the `reference.restart` file that we saved earlier
{: .step}
> A suffix to apply to the data will be suggested based on the filename, in this case 'reference', which is fine, so click _OK_
{: .step}
> Click _close_ to exit the _Data Manager_ dialog
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

[Previous Step](step9.md){: .btn }   [Next Step](step9b.md){: .btn .right}
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
## 9. Compare Data

Viewing the output of modules in their own tabs is convenient enough, but if we want to compare data from different sources (or different restart points) on the same plot we need to use a workspace. A workspace is an empty tab to which you can add various "gizmos" to do graphing and further data analysis. Note that the contents of workspaces are not saved to the [main input file](../../userguide/inputfile) - they are stored in the [GUI state file](../../userguide/outputfiles/state.md).

In the next two sections we'll compare both the centre-of-geometry RDF and the axis angle map between the reference and refined simulations.

[Previous Step](step8.md){: .btn }   [Next Step](step9a.md){: .btn .right}
