---
title: Step 4 - Equilibrate the System
weight: 6
---


We're now in a position to run our initial equilibration of the system, so let's set the simulation running.

{{< action type="mouse" text=" Click the **Run** button on the left-hand side of the main window" >}}
{{< action type="menu" text="Simulation &#8680; Run" >}}
{{< action type="key" text="Ctrl-R" >}}


It's useful to observe the energy of the configuration as we're equilibrating our system, since once this reaches a steady value we can begin to think about what to do next. We'll open up our [`Energy`](../../userguide/modules/energy) module in a separate tab so we can see the plot of the various energy components

{{< action type="tabs" text="**Evolve (Standard)** tab" >}}
{{< step text="Double-click on the [`Energy`](../../userguide/modules/energy) module to open its additional controls in a separate tab">}}


You'll see several curves on the graph, including the total energy (black line) which is the sum of the internal (intramolecular) bond and angle and the intermolecular (i.e. Lennard-Jones plus coulomb) terms. The [`MD`](../../userguide/modules/md) module is set to only run if the total energy of the configuration is stable, but you should be able to notice when it kicks in as the bond (green line), angle (purple line), and total (blue) intramolecular energies which are constant during the Monte Carlo steps will suddenly drop to lower values. There will be some variation of the components up and down, but for a system of 1000 water molecules the total energy should approach a value of around -35&times;10<sup>3</sup> kJ mol<sup>-1</sup> with the intermolecular energy around -44&times;10<sup>3</sup> kJ mol<sup>-1</sup>.

The energy will take a few hundred iterations to stabilise, so while we're waiting we'll take an initial look at the comparison between simulation and experiment in the next section.

{{< button pos="left" text="Previous Step" path= "../step3/">}}
{{< button pos="right" text="Next Step" path= "../step5/">}}
