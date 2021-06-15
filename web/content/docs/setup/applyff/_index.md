---
title: Applying Forcefields
description: Giving life to a species by applying a forcefield
weight: 4
---

Forcefields are the key input to any classical simulation code, and almost completely determine the "quality" of the simulation itself. What is "quality"? Commonly, one judges a particular forcefield on how it reproduces some experimentally-measured properties. However, forcefields are seldom tested against *all* available data, and indeed the purpose of some forcefields is to reproduce specific properties without regard to others. The point is to understand what the forcefield you are applying is good for, and more importantly if it is relevant for what you are trying to do.

Of course, one of the principal functions of Dissolve is to perform potential refinement _a la_ EPSR, so in a loose sense the input forcefield doesn't have to be perfect as the refinement procedure will generate the necessary corrections. However, in practice it is *always* advisable to try and find the best forcefield for your species / system as you can and use that as the starting point. This will save a lot of time in the refinement part of the code, which then only has to work on generating small corrections to the input potentials.
