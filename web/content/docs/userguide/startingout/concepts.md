---
title: Basic Concepts of Dissolve
description: "Some basic information introducing the software"
---
{{< label color="blue" icon="fa-clock" text="5 minute read" >}}

## Overview

Dissolve's primary goal is to generate an ensemble of atomic configurations which is consistent (as far as is possible) with reference experimental data, and to use this ensemble to extract a structural picture of the system that cannot directly be determined from the raw experimental data.

Dissolve was designed from the outset to be able to handle large systems with millions of atoms, multiple configurations within the same simulation, and distributed parallel execution. It is also intended to offer a balance between flexibility (it can be tailored to suit specific problems) and usability (it will help you do things wherever possible).

* * *

## Requirements

Apart from some experimental data to work with, Dissolve requires that you describe the system you're interested in in terms of the interaction between atoms and molecules, just as you would in a dedicated classical simulation package. This means that you must use, or provide, a forcefield that gives a reasonable representation of the structure of your system. Dissolve has a growing number of built-in forcefields, but you may need to enter that information by hand, or speak nicely with the development team about getting some help to add it in to the code.

* * *

## Terminology

Setting up Dissolves main simulation typically relies on the following data to be provided (in this order).

### Master Terms (optional)

Intramolecular terms such as bond and angle stretches that are used frequently within molecules can be defined at the beginning of the simulation and then referenced later on. This means that, if you want to go back and change a bond length for instance, you only have to do so in one place, rather than over multiple bonds in one (or more) molecules.

Master terms are defined in the [`MasterTerms`](../../inputfile/master) block of the [main input file](../../inputfile/).

### Species

A Species object provides all the geometry definitions for a single atomic or molecular species to be used during the course of the simulation. In the current version of Dissolve, they also contain definitions for isotopic versions (isotopologues) of the same species.

A Species is defined in a [`Species`](../../inputfile/species) block of the [main input file](../../inputfile/).

### Configuration

A Configuration represents a set of atomic coordinates representing the positions of one or many individual 'copies' of one or more Species. Only the atomic positions are stored in the Configuration - when intramolecular terms are required Dissolve refers back to the original Species definition. A Configuration typically has a Box defined for it in order to provide periodic boundary conditions.

A Configuration is defined in a [`Configuration`](../../inputfile/configuration) block of the [main input file](../../inputfile/).

### Layer

Layers (or Processing Layers) are where the magic happens. They contain one or more Modules that perform specific tasks (usually) on Configurations. For instance, there are Modules that construct or evolve the coordinates of configurations, evaluate energies and forces, calculate neutron-weighted structure factors, and perform specific analytic functions to deliver quantities and properties of interest. Layers run Modules in a predefined sequence to deliver the desired intent of the overall simulation.

Main processing layers are defined in a [`Layer`](../../inputfile/layer) block of the [main input file](../../inputfile/). Configurations also contain a predefined Layer to which Modules may be added, and which are run before the main processing loop begins.

* * *

## Building a Simulation

Owing to the variety of systems that Dissolve can potentially be applied to, its main simulation loop can be constructed and configured on a case-by-case basis. Suitable evolution algorithms can be put in place for specific configurations (and even specific molecules), and the order of operation is completely up to the user. Of course, there are several "standard" simulations that one can think of, and Dissolve provides skeletal set ups for these and others (see [System Templates](../../setup/scratch)) for you to "fill in the blanks".

