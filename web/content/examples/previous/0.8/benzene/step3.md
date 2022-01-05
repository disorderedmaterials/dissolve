---
title: Step 3 - Set up Equilibration
type: docs
---


To equilibrate our system we'll now add a standard Monte Carlo / Molecular Dynamics processing layer:

{{< action type="menu" text="Layer &#8680; Create... &#8680; Evolution... &#8680; Standard Molecular (MC/MD)" >}}


We can now start running our simulation and start to equilibrate the configuration box.

{{< action type="mouse" text="Click the **Run** button on the left-hand side of the main window" >}}
{{< action type="menu" text="Simulation &#8680; Run" >}}
{{< action type="key" text="Cntrl-R" >}}


If you're watching the box in the configuration tab you'll notice that it intially "explodes" because of our defined size factor of 10.0, but the box will quickly reduce in size back to normal and result in a condensed phase with no interlocked benzene rings.

When the size factor has reduced the inter-molecular energy of the simulation will fluctuate a little, but should oscillate around some equilibrium value, at which point you can proceed. You can check this from the graphs of the [`Energy`]({{< ref "energy" >}}) module:

{{< action type="tabs" text="**Evolve (Standard)** layer tab" >}}
{{< action type="mouse" text="Double-click the `Energy` module to open it in a separate tab">}}

Once this has been achieved, we can make a snapshot of the current simulation and store this equilibrated point as a backup. The restart file written by Dissolve as it runs stores all the information necessary to continue a stopped simulation, but there is no going back to a previous point in the simulation (e.g. to undo a bad analysis choice, or to reference earlier data) unless we have a suitable restart point to go back to. At any time we can write a new restart file from the GUI, independent of the standard restart file, and which we can keep to load in at a later date.

{{< action type="menu" text="Simulation &#8680; Save Restart Point..." >}}
{{< action type="edit" text="Change the filename to `equilibrated.restart` and click _Save_" >}}

Now we can move on to set up the analysis of the structural properties we're interested in.

{{< button pos="left" text="Previous Step" path="step2/">}}
{{< button pos="right" text="Next Step" path="step4/">}}
