---
title: Simulation Overview
weight: 1
description: "Basic components in a simulation"
---
{{< label color="blue" icon="fa-clock" text="5 minute read" >}}

Setting up Dissolve's main simulation typically relies on the following data to be provided (in this order).

### Master Terms (optional)

Intramolecular terms such as bond and angle stretches that are used frequently within molecules can be defined at the beginning of the simulation and then referenced later on. This means that, if you want to go back and change a bond length for instance, you only have to do so in one place, rather than over multiple bonds in one (or more) molecules.

Master terms are defined in the [`MasterTerms`]({{< ref "masterblock" >}}) block of the [main input file]({{< ref "inputfile" >}}).

### Species

A Species object provides all the geometry definitions for a single atomic or molecular species to be used during the course of the simulation. In the current version of Dissolve, they also contain definitions for isotopic versions (isotopologues) of the same species.

A Species is defined in a [`Species`]({{< ref "speciesblock" >}}) block of the [main input file]({{< ref "inputfile" >}}).

### Configuration

A Configuration represents a set of atomic coordinates representing the positions of one or many individual 'copies' of one or more Species. Only the atomic positions are stored in the Configuration - when intramolecular terms are required Dissolve refers back to the original Species definition. A Configuration typically has a Box defined for it in order to provide periodic boundary conditions.

A Configuration is defined in a [`Configuration`]({{< ref "configurationblock" >}}) block of the [main input file]({{< ref "inputfile" >}}).

### Layer

Layers (or Processing Layers) are where the magic happens. They contain one or more Modules that perform specific tasks (usually) on Configurations. For instance, there are Modules that construct or evolve the coordinates of configurations, evaluate energies and forces, calculate neutron-weighted structure factors, and perform specific analytic functions to deliver quantities and properties of interest. Layers run Modules in a predefined sequence to deliver the desired intent of the overall simulation.

Main processing layers are defined in a [`Layer`]({{< ref "layerblock" >}}) block of the [main input file]({{< ref "inputfile" >}}). Configurations also contain a predefined Layer to which Modules may be added, and which are run before the main processing loop begins.
