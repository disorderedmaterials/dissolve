---
title: 3. Setting up Dissolve
description: Constructing a simulation to do what you want
weight: 3
---
At its core Dissolve is a classical molecular simulation code, describing interactions between atoms with parameterised interaction potentials which approximate those interactions in the real system. Together, these potentials describe the behaviour of individual molecules, and how they interact with each other, allowing larger systems representing your system of interest to be constructed. Providing a complete description of your various types of molecule etc. is a core part of getting Dissolve (and indeed any classical simulation code) to run in a realistic manner.

Of course, your real system contains a rather frightening number of individual atoms - even a cubic micron (one thousandth of a millimetre) of water contains of the order of a hundred billion of them - and while simulating such vast numbers of particles would be nice, this is not a practical prospect for common scientific investigation. So, most all codes like Dissolve rely on simulating a _representative_ small-scale system, which captures the general features of the real system, but which will lack many of the nuances of it.

The following sections will help to describe what the various components are, how to generate, create, or obtain them, and how to put them all together to make a useful simulation. Most of what follows is relevant to the Dissolve GUI, which is the primary tool for setting up a simulation.
