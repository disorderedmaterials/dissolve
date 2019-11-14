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

Most modules need configuration targets to work on, and all available configurations (one in our case) will have been automatically set as a target in each of the new modules in the layer. Each module has its own defined frequency at which it will run, and can be temporarily disabled if necessary. There are usually a bunch of settings (accessed via the cog button) that control its action and, most importantly, the module may have entire control panels of other functionality (e.g. graphing output for various properties etc.). The latter cannot be viewed in layer tabs - instead, double-click on the module block to open an additional tab containing these controls, in addition to all of its settings.

All of the default settings for the modules within the evolution layer are sensible enough for our needs, so take a look around at what's there if you want to, but there's no need to change anything.

[Previous Step](step4.md){: .btn }   [Next Step](step4b.md){: .btn .right}
