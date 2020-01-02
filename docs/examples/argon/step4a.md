---
title: Step 4a - Evolve the System
grand_parent: Examples
parent: Liquid Argon
nav_order: 5
---
# Liquid Argon

## 4a) Evolving the System

We almost always want to move the contents of our configurations around, and this is the job of the evolution layer.

> Layer &#8680; Create... &#8680; Evolution... &#8680; Standard Atomic (MC/MD)
{: .action .action_menu}

Our new layer contains the following modules:

| Module | Purpose |
|--------|---------|
| [`AtomShake`](/userguide/modules/atomshake) | Performs standard Monte Carlo moves on individual atoms, one per atom |
| [`MD`](/userguide/modules/md) | Performs a number of molecular dynamics steps, evolving the system according to Newton's equations of motion |
| [`Energy`](/userguide/modules/energy) | Calculates the total energy, including contributions from intramolecular terms if present |

Selecting any module in the list on the left will show its full options in the panel on the right - select the [`AtomShake`](/userguide/modules/atomshake) to see the target acceptance ratio, step size, etc.

Most modules need configuration targets to work on, and all available configurations (one in our case) will have been automatically set as a target in each of the new modules in the layer. As mentioned, each module has its own defined frequency at which it will run within the layer - [`AtomShake`](/userguide/modules/atomshake) and [`Energy`](/userguide/modules/energy) modules will run every time the layer is run (frequency = 1) while the [`MD`](/userguide/modules/md) module will only run every fifth step. Each module can be individually enabled / disabled using the slider, with a green tick indicating that the module is active.

Finally, modules may have entire control panels of other functionality, most commonly graphing output for various properties etc. This cannot be viewed within a layer tab - instead, double-click on the module block to open an additional tab containing these controls, in addition to all of its settings.

All of the default settings for the modules within the evolution layer are sensible enough for our needs, so take a look around at what's there if you want to, but there's no need to change anything.

[Previous Step](step4.md){: .btn }   [Next Step](step4b.md){: .btn .right}
