---
title: User Guide
has_children: true
nav_order: 30
has_toc: false
---
# User Guide

## 1) Introduction

[What is Dissolve?](overview.md) - What Dissolve is, and what Dissolve isn't.

* * *

## 2) Starting Out

[Concepts](startingout/concepts.md) - An overview to the basic concepts of Dissolve.

[The GUI](startingout/gui.md) - Brief introduction to the Dissolve GUI.

[Workflow](startingout/workflow.md) - Overview of the basic input, operation, and output of Dissolve.

[First Simulation](../examples/argon/) - Liquid Argon.

* * *

## 3) Main Input File

[Overview](inputfile/index.md) - Dissolve's main input file explained.

[Configuration Block](inputfile/configuration.md) - Defining configurations in the simulation.

[Layer Block](inputfile/layer.md) - Defining layers to do useful work.

[Master Block](inputfile/master.md) - Setting master intramolecular terms for use across species.

[Module Block](inputfile/module.md) - Defines settings and control options for a module.

[PairPotential Block](inputfile/pairpotential.md) - Define interaction parameters for use in the simulation.

[Simulation Block](inputfile/simulation.md) - Control the general behaviour of Dissolve.

[Site Block](inputfile/site.md) - Define a site on a species for use in analysis routines.

[Species Block](inputfile/species.md) - Define a species for use in the simulation.

* * *

## 4) Setting up Dissolve

[Using the GUI](setup/scratch.md) - Constructing simulations using the GUI.

[System Templates](setup/templates.md) - Skeletal input files upon which to build your system.

* * *

## 5) Running Dissolve

[CLI Options](run/cli.md) - All of Dissolve's command-line options explained.

[Dissolve on SCARF](run/scarf.md) - Where to find, and how to run, Dissolve on SCARF.

* * *

## 6) Output Files

[Heartbeat File](heartbeatfile.md) - Purpose and content of the heartbeat file.

[Restart File](restartfile.md) - Format and content of the restart file.

* * *

## 7) Modules

[Overview](modules/index.md) - Modules main index.

### Analysis - Calculation of properties of interest from configurations.

#### General
[Analyse](modules/analyse/index.md) - Perform custom analysis on configurations.

#### Two-Centre Correlation Functions
[CalculateRDF](modules/calculaterdf/index.md) - Calculate site-site radial distribution functions.

#### Three-Centre Correlation Functions
[CalculateDAngle](modules/calculatedangle/index.md) - Calculate distance-angle maps.

### Checks & Tests - Modules for checking, testing, and ensuring consistency.

[Checks](modules/checks/index.md) - Perform basic testing of geometries etc..

[DataTest](modules/datatest/index.md) - Compare calculated data from modules with external references.

[SanityCheck](modules/sanitycheck/index.md) - Conduct internal sanity checking between all processes (parallel only).

### Correlation Functions - Methods to calculate real- and reciprocal-space correlation functions.

[NeutronSQ](modules/neutronsq/index.md) - Calculate neutron-weighted S(Q) and G(r).

[RDF](modules/rdf/index.md) - Calculate standard radial distribution functions between all atom types.

[SQ](modules/sq/index.md) - Calculate unweighted S(Q) and G(r).

### Evolution - Methods to evolve the coordinates of configurations.

[AtomShake](modules/atomshake/index.md) - Perform Monte Carlo on individual atoms.

[IntraShake](modules/intrashake/index.md) - Perform Monte Carlo on intramolecular terms.

[MD](modules/md/index.md) - Run a short molecular dynamics simulation.

[MolShake](modules/molshake/index.md) - Perform Monte Carlo on whole molecules.

### Forcefield - Energy evaluation, force calculation, and forcefield modification.

[Energy](modules/energy/index.md) - Calculate total system energy and its contributions.

[EPSR](modules/EPSR.md) - Refine interatomic potentials according to the methodology of Soper.

[Forces](modules/forces/index.md) - Calculate atomic forces for configurations.

### Input & Output - Import and export various data, including configuration coordinates and trajectories.

[Export](modules/export/index.md) - Export various quantities to file (including configurations).

[Import](modules/import/index.md) - Import data to simulation (e.g. trajectories).

### Optimisation - Minimisation and fitting modules.

[Calibrate](modules/calibrate/index.md) - Adjust various quantities automatically.

[GeometryOptimisation](modules/geometryoptimisation/index.md) - Run an energy minimisation in order to optimise geometry.

