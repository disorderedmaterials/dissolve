---
title: Step 6 - Next Steps
grand_parent: Examples
parent: Liquid Argon
nav_order: 8
---
# Liquid Argon

## 6) Next Steps

Your first real simulation is now complete. The agreement with experimental data achieved by the reference potential isn't bad at all, but it could be slightly better.

As an illustrative challenge you could try to adjust the forcefield parameters by hand to see if you can reduce the error between experiment and simulation. Our current system is a pure Lennard-Jones fluid, and is one of the simplest example systems that exist. As such, there are only two adjustable parameters in the simulation - the epsilon and sigma values that describe the Lennard-Jones potential between Ar atoms.

With the simulation stopped you can go to the _Forcefield_{: .text-green-100} tab and change one of either epsilon (Parameter 1) or sigma (Parameter 2). The former reflects the well-depth of the potential and controls how strongly the particles interact, while the sigma value controls the real-space distance at which the minimum of the well is found. Change one of these values (increasing epsilon by 10% or so may be a good starting point...) and run the simulation for a little while until the g(r) and S(Q) stabilise, then assess how good the agreement is and judge your next move.

This kind of manual adjustment is possible, albeit tedious, for simplistic systems, but quickly becomes impractical. Toth[1] has previously conducted a study where the Lennard-Jones parameters for liquid argon were optimised against the reference data of Yarnell that we used in this example. If

### References
[1] "Determination of pair-potential parameters from experimental structure factors", G. Toth, _J. Chem. Phys._ **115* (2001).

[Previous Step](step5.md){: .btn }
