---
title: Step 4 - Set up the Simulation
---


It's time to tell Dissolve exactly what we want to do with this argon system. All of Dissolve's "useful" work is achieved by sequences of [modules](../../userguide/modules), with each module performing a specific task, function, or calculation. Modules exist within 'layers', and it usually makes sense to break up simulations into distinct parts (e.g. keeping evolution of the system separate from refinement or analysis) as layers can be turned on and off as the situation requires. Each layer also has a frequency at which it will run, relative to the main simulation loop counter - a frequency of 1 means the layer will be executed every iteration. Modules within the layer also have an associated run frequency, but this is relative to the number of times the layer has been executed, rather than the main loop counter.

We'll split our simulation up into two layers:

1. An 'evolution' layer which moves the contents of our configuration around and calculates the total energy of the system
2. A 'calculation' layer which periodically calculates the radial distribution function and neutron-weighted structure factor

{{< tip text="You can have as many layers in a simulation as you need - whatever makes the most sense in what you're trying to do.">}}


{{< button pos="left" text="Previous Step" path="../step3/">}}
{{< button pos="right" text="Next Step" path="../step4a/">}}
