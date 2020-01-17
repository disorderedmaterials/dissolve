---
title: Step 3 - Set up Equilibration
parent: Comparing Benzene Structure
grand_parent: Examples
---
# Comparing Benzene Structure

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

When the size factor has reduced the total energy of the simulation will fluctuate a little, but should oscillate around some equilibrium value, at which point you can proceed. You can check this from the graphs of the [`Energy`](../../userguide/modules/energy) module:

Once this has been achieved, we can make a snapshot of the current simulation and store this equilibrated point as a backup. The restart file written by Dissolve as it runs stores all the information necessary to continue a stopped simulation, but there is no going back to a previous point in the simulation (e.g. to undo a bad analysis choice, or to reference earlier data) unless we have a suitable restart point to go back to. At any time we can write a new restart file from the GUI, independent of the standard restart file, and which we can keep to load in at a later date.

> Simulation &#8680; Save Restart Point...
{: .action .action_menu}
> Change the filename to `equilibrated.restart` and click _Save_
{: .action .action_edit}

Now we can move on to set up the analysis of the structural properties we're interested in.

[Previous Step](step2.md){: .btn }   [Next Step](step4.md){: .btn .right}
