---
title: Simulation Overview
weight: 1
description: "Basic components in a simulation"
---

Setting up Dissolve's main simulation typically requires on the following data to be provided (in this order).

## Master Terms (optional)

Intramolecular terms such as bond and angle stretches that are used frequently within molecules can be defined at the beginning of the simulation and then referenced later on. This means that, if you want to go back and change a bond length for instance, you only have to do so in one place, rather than over multiple bonds in one (or more) molecules. As an example, consider the benzene molecule - there are six distinct C-C bonds, but due the symmetry and chemical similarity of those carbon atoms, all the bonds can be described by the same interaction parameters. Thus, you can specify those once in a master term, and apply that term to all six C-C bonds.

Master terms are defined in the [`MasterTerms`]({{< ref "masterblock" >}}) block of the [main input file]({{< ref "inputfile" >}}).

## Species

A _species_ provides all the geometry definitions for a single atomic or molecular species to be used during the course of the simulation. In the current version of Dissolve, they also contain definitions for isotopic variants (isotopologues) of the same species.

A species is defined in a [`Species`]({{< ref "speciesblock" >}}) block of the [main input file]({{< ref "inputfile" >}}).

There are several ways to bring a species in to Dissolve - see [Defining Species]({{< ref "species" >}}) for more information.

## Configuration

A _configuration_ represents a set of atomic coordinates representing the positions of one or many individual 'copies' of one or more species. Only the atomic positions are stored in the Configuration - when intramolecular terms are required Dissolve refers back to the original species definition. A configuration typically has a Box defined for it in order to provide periodic boundary conditions.

A Configuration is defined in a [`Configuration`]({{< ref "configurationblock" >}}) block of the [main input file]({{< ref "inputfile" >}}).

## Layer

Layers (or _processing layers_) are where the magic happens, and contain one or more [modules]({{< ref "modules" >}}) that perform specific tasks. For instance, there are modules that construct or evolve the coordinates of configurations, evaluate energies and forces, calculate neutron-weighted structure factors, and perform specific analytic functions to deliver quantities and properties of interest. Layers run modules in a predefined sequence, and at a predefined frequency, in order to deliver the desired intent of the overall simulation.

Main processing layers are defined in a [`Layer`]({{< ref "layerblock" >}}) block of the [main input file]({{< ref "inputfile" >}}).
