---
title: User Guide
has_children: true
nav_order: 30
has_toc: false
---
# User Guide

## 1) Introduction

[**What is Dissolve?** - What Dissolve is, and what Dissolve isn't](overview.md)

* * *

## 2) Starting Out

[**Concepts** - An overview to the basic concepts of Dissolve](startingout/concepts.md)
[**The GUI** - Brief introduction to the Dissolve GUI](startingout/gui.md)
[**Workflow** - Overview of the basic input, operation, and output of Dissolve](startingout/workflow.md)

[**First Simulation** - Liquid Argon](../examples/argon/)

* * *

## 3) Main Input File

[**Overview** - Dissolve's main input file explained](inputfile/index.md)

[**Configuration Block** - Defining configurations in the simulation](inputfile/configuration.md)
[**Layer Block** - Defining layers to do useful work](inputfile/layer.md)
[**Master Block** - Setting master intramolecular terms for use across species](inputfile/master.md)
[**Module Block** - Defines settings and control options for a module](inputfile/module.md)
[**PairPotential Block** - Define interaction parameters for use in the simulation](inputfile/pairpotential.md)
[**Simulation Block** - Control the general behaviour of Dissolve](inputfile/simulation.md)
[**Site Block** - Define a site on a species for use in analysis routines](inputfile/site.md)
[**Species Block** - Define a species for use in the simulation](inputfile/species.md)

* * *

## 4) Setting up Dissolve

[**Using the GUI** - Constructing simulations using the GUI](setup/scratch.md)
[**System Templates** - Skeletal input files upon which to build your system](setup/templates.md)

* * *

## 5) Running Dissolve

[**CLI Options** - All of Dissolve's command-line options explained](run/cli.md)
[**Dissolve on SCARF** - Where to find, and how to run, Dissolve on SCARF](run/scarf.md)

* * *

## 6) Output Files

[**Heartbeat File** - Purpose and content of the heartbeat file](heartbeatfile.md)
[**Restart File** - Format and content of the restart file](restartfile.md)

* * *

## 7) Modules

[**Overview** - Modules main index](modules/index.md)

### Analysis - Calculation of properties of interest from configurations.

#### General
[**Analyse** - Perform custom analysis on configurations](modules/analyse/index.md)

#### Two-Centre Correlation Functions
[**CalculateRDF** - Calculate site-site radial distribution functions](modules/calculaterdf/index.md)

#### Three-Centre Correlation Functions
[**CalculateDAngle** - Calculate distance-angle maps](modules/calculatedangle/index.md)

### Checks & Tests - Modules for checking, testing, and ensuring consistency.

[**Checks** - Perform basic testing of geometries etc.](modules/checks/index.md)
[**DataTest** - Compare calculated data from modules with external references](modules/datatest/index.md)
[**SanityCheck** - Conduct internal sanity checking between all processes (parallel only)](modules/sanitycheck/index.md)

### Correlation Functions - Methods to calculate real- and reciprocal-space correlation functions.

[**NeutronSQ** - Calculate neutron-weighted S(Q) and G(r)](modules/neutronsq/index.md)
[**RDF** - Calculate standard radial distribution functions between all atom types](modules/rdf/index.md)
[**SQ** - Calculate unweighted S(Q) and G(r)](modules/sq/index.md)

### Evolution - Methods to evolve the coordinates of configurations.

[**AtomShake** - Perform Monte Carlo on individual atoms](modules/atomshake/index.md)
[**IntraShake** - Perform Monte Carlo on intramolecular terms](modules/intrashake/index.md)
[**MD** - Run a short molecular dynamics simulation](modules/md/index.md)
[**MolShake** - Perform Monte Carlo on whole molecules](modules/molshake/index.md)

### Forcefield - Energy evaluation, force calculation, and forcefield modification.

[**Energy** - Calculate total system energy and its contributions](modules/energy/index.md)
[**EPSR** - Refine interatomic potentials according to the methodology of Soper](modules/EPSR.md)
[**Forces** - Calculate atomic forces for configurations](modules/forces/index.md)

### Input & Output - Import and export various data, including configuration coordinates and trajectories.

[**Export** - Export various quantities to file (including configurations)](modules/export/index.md)
[**Import** - Import data to simulation (e.g. trajectories)](modules/import/index.md)

### Optimisation - Minimisation and fitting modules.

[**Calibrate** - Adjust various quantities automatically](modules/calibrate/index.md)
[**GeometryOptimisation** - Run an energy minimisation in order to optimise geometry](modules/geometryoptimisation/index.md)

