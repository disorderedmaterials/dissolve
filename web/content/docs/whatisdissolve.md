---
title: 1. Introduction
description: A brief overview of the purpose of Dissolve
weight: 1
---
{{< label color="blue" icon="fa-clock" text="2 minute read" >}}

Dissolve is a code to help make sense of experimental structural scattering data by generating suitable representations ('models') of the real-world systems that are consistent with the measured data. These models are typically atomistic in nature (i.e. the atom is the basic unit that is assembled to make up the model), and employ classical forcefields in order to describe the interactions within. These interaction parameters are then adjusted by some method in order to drive agreement between the measured data and that calculated from the atomistic representation. As such, Dissolve sits somewhere in-between being a classical simulation code (doing standard classical simulation things such as Monte Carlo and molecular dynamics) and an optimisation package. 

The data to be fit typically derive from neutron (total) scattering experiments, where isotopic substitution is employed in order to gather multiple datasets describing the same underlying structure.

### So, I just give it some experimental data and it gives me a model describing it in terms of atoms?

Not quite. You still must describe your system in terms of composition, the structure of the basic molecules comprising your system, density etc.

### And **then** I get a model describing my data?

Potentially (no pun intended). You also need to provide a starting description of the interactions in the system (i.e. a forcefield). Dissolve then offers methods to adjust and refine the supplied potentials so as to promote agreement betwen the scattering profiles calculated from the simulation, and those you provided as the data.

### I see. Does it always work?

A good fit is not guaranteed. Experimental systems are typically complex, consisting of multiple different species, phases etc. There is only so much isotopic substitution that is possible to perform, and there are always errors associated with experimental measurement and data analysis. However, Dissolve will try its best to get a good answer.

### But what if I want a perfect fit to my data?

Then you are most likely out of luck. As mentioned already, there are always errors associated with data collection and processing, and a perfect fit to the data should never be expected. A good baseline expectation is that, once Dissolve has done its thing, the disagreement between the refined simulation and the experimental data will be smaller than that between the original starting forcefield and the experimental data.

### This sounds a lot like EPSR. Is it the same?

EPSR (Empirical Potential Structure Refinement), written by Prof. Alan Soper, paved the way for structural characterisation of disordered systems measured by neutron diffraction. Dissolve is definitely similar in spirit and should be considered as a “partner” in the field of such analysis. It reproduces parts of EPSR so that it can, to some extent, perform exactly the same processes as EPSR. However, it also provides the tools to treat much larger and more complex systems than EPSR, and with more flexibility in its approach.
