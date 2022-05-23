---
title: Step 4a - Evolve the System
type: docs
---


We almost always want to move the contents of our configurations around, and this is the job of the evolution layer.

{{< action type="menu" text="Layer &#8680; Create... &#8680; Evolution... &#8680; Standard Atomic (MC/MD)" >}}


Our new layer contains the following modules:

| Module | Purpose |
|--------|---------|
| [`AtomShake`]({{< ref "atomshake" >}}) | Performs standard Monte Carlo moves on individual atoms |
| [`MD`]({{< ref "md" >}}) | Performs a number of molecular dynamics steps, evolving the system according to Newton's equations of motion |
| [`Energy`]({{< ref "energy" >}}) | Calculates the total energy, including contributions from intramolecular terms if present |

Selecting any module in the list on the left will show its full options in the panel on the right - select the [`AtomShake`]({{< ref "atomshake" >}}) to see its list of options. [`AtomShake`]({{< ref "atomshake" >}}) performs standard atomic Monte Carlo on one or more target configurations. It tries to move each atom in the configuration in turn, randomly displacing it by a certain amount, and checking the energy to see if that displacement constituted a 'good' move. As such it has options like **TargetAcceptanceRate**, which governs the number of moves that can / should be accepted in any one cycle, and **StepSize** which indicates the maximum distance by which an atom can be moved in any translation attempt (over time this will change automatically to give the acceptance ratio requested).

Most modules need configuration targets to work on, and all available configurations (one in our case) will have been automatically set as a target in each of the new modules in the layer. As mentioned, each module has its own defined frequency at which it will run within the layer - [`AtomShake`]({{< ref "atomshake" >}}) and [`Energy`]({{< ref "energy" >}}) modules will run every time the layer is run (frequency = 1) while the [`MD`]({{< ref "md" >}}) module will only run every fifth step. Each module can be individually enabled / disabled using the slider next to the frequency box, with a green tick indicating that the module is active.

Finally, modules may have entire control panels of other functionality, most commonly graphing output for various properties etc. This can be accessed by selecting the **Output** button at the top of the controls. Go back to the configuration options for the module by selecting the **Options** button.

All of the default settings for the modules within the evolution layer are sensible enough for our needs, so take a look around at what's there if you want to, but there's no need to change anything.


{{< button pos="left" text="Previous Step" path="step4/">}}
{{< button pos="right" text="Next Step" path="step4b/">}}
