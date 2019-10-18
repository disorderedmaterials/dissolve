---
title: Step 4 - Set up the Simulation
grand_parent: Learn Dissolve
parent: Liquid Argon
nav_order: 4
---
# Liquid Argon

## 4) Set up the Simulation

It's time to tell Dissolve exactly what we want to do with this argon system, so we add some processing layers to the simulation. It makes sense to break up the simulation in to distinct parts (e.g. keeping evolution of the system separate from refinement or analysis) as layers can be turned on and off as the situation required. Each layer also has a frequency at which it will run, relative to the main simulation loop counter - a frequency of 1 means the layer will be executed every iteration. Modules within the layer also have an associated run frequency, but this is relative to the number of times the layer has been executed, rather than the main loop counter.

We'll split our simulation up in to three layers:

1. An 'evolution' layer which moves the contents of our configuration around and calculates the total energy of the system
2. A 'calculation' layer which periodically calculates the radial distribution function and structure factor
3. A 'refinement' layer which performs our potential refinement and drives the simulation towards agreement with the experimental data

> You can have as many layers in a simulation as you need - whatever makes the most sense in what you're trying to do.
{: .tip}

[Previous Step](step3.md){: .btn }   [Next Step](step4a.md){: .btn .right}
