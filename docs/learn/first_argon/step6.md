---
title: Step 6 - Refine the Potential
grand_parent: Learn Dissolve
parent: Liquid Argon
nav_order: 8
---
# Liquid Argon

## 6) Refining the Potential

Our final processing layer will handle the adjustment of the interatomic potential. Go to the _Layer_{: .text-green-100} menu and choose _Create...→Evolution...→Standard Atomic (MC/MD)_{: .text-green-100}. A new layer will be created containing a single module:

| Module | Purpose |
|--------|---------|
| [`EPSR`](/modules/epsr) | Performs potential refinement using the core methodology of Soper's EPSR |

The [`EPSR`](/modules/epsr) module does not target any configurations - rather, it works on a set of weighted structure factors (e.g. those calculated by [`NeutronSQ`](/modules/neutronsq) modules). By default, 

[Previous Step](step5.md){: .btn }   [Next Step](step7.md){: .btn .right}
