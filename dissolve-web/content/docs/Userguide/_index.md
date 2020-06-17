---
title: User Guide
weight: 10
has_toc: false
---

## 1) Introduction

[What is Dissolve?](docs/userguide/overview) - What Dissolve is, and what Dissolve isn't.

* * *

## 2) Starting Out

[Concepts](docs/userguide/startingout/concepts) - An overview to the basic concepts of Dissolve.

[The GUI](docs/userguide/startingout/gui) - Brief introduction to the Dissolve GUI.

[Workflow](docs/userguide/startingout/workflow) - Overview of the basic input, operation, and output of Dissolve.

[Units](docs/userguide/startingout/units) - Units used throughout Dissolve.

[First Simulation](../examples/argon/) - Step-by-step set-up of a simple liquid argon simulation.

* * *

## 3) Main Input File

[Overview](docs/userguide/inputfile/index) - Dissolve's main input file explained.

[Configuration Block](docs/userguide/inputfile/configuration) - Defining configurations in the simulation.

[Layer Block](docs/userguide/inputfile/layer) - Defining layers to do useful work.

[Master Block](docs/userguide/inputfile/master) - Setting master intramolecular terms for use across species.

[Module Block](docs/userguide/inputfile/module) - Defines settings and control options for a module.

[PairPotential Block](docs/userguide/inputfile/pairpotential) - Define interaction parameters for use in the simulation.

[Simulation Block](docs/userguide/inputfile/simulation) - Control the general behaviour of Dissolve.

[Site Block](docs/userguide/inputfile/site) - Define a site on a species for use in analysis routines.

[Species Block](docs/userguide/inputfile/species) - Define a species for use in the simulation.

* * *

## 4) Setting up Dissolve

[Using the GUI](docs/userguide/setup/scratch) - Constructing simulations using the GUI.

[System Templates](docs/userguide/setup/templates) - Skeletal input files upon which to build your system.

* * *

## 5) Running Dissolve

[CLI Options](docs/userguide/run/cli) - All of Dissolve's command-line options explained.

[Dissolve on SCARF](docs/userguide/run/scarf) - Where to find, and how to run, Dissolve on SCARF.

* * *

## 6) Output Files

[Heartbeat File](docs/userguide/outputfiles/heartbeat) - Purpose and content of the heartbeat file.

[Restart File](docs/userguide/outputfiles/restart) - Format and content of the restart file.

[GUI State File](docs/userguide/outputfiles/state) - Format and content of the restart file.

* * *

## 7) Modules

[Overview](docs/userguide/modules/) - Modules main index.

### Analysis - Calculation of properties of interest from configurations.

#### General
[Analyse](docs/userguide/modules/analyse/) - Perform custom analysis on configurations.

#### Geometry
[CalculateAvgMol](docs/userguide/modules/calculateavgmol/) - Calculate the average geometries of molecules

#### Two-Centre Correlation Functions
[CalculateCoordinationNumber](docs/userguide/modules/calculatecn/) - Calculate coordination numbers from a radial distribution function.
[CalculateRDF](docs/userguide/modules/calculaterdf/) - Calculate site-site radial distribution functions.

#### Three-Centre Correlation Functions
[CalculateDAngle](docs/userguide/modules/calculatedangle/) - Calculate distance-angle maps.

### Checks & Tests - Modules for checking, testing, and ensuring consistency.

[Checks](docs/userguide/modules/checks/) - Perform basic testing of geometries etc..

[DataTest](docs/userguide/modules/datatest/) - Compare calculated data from modules with external references.

[SanityCheck](docs/userguide/modules/sanitycheck/) - Conduct internal sanity checking between all processes (parallel only).

### Correlation Functions - Methods to calculate real- and reciprocal-space correlation functions.

[NeutronSQ](docs/userguide/modules/neutronsq/) - Calculate neutron-weighted S(Q) and G(r).

[RDF](docs/userguide/modules/rdf/) - Calculate standard radial distribution functions between all atom types.

[SQ](docs/userguide/modules/sq/) - Calculate unweighted S(Q) and G(r).

### Evolution - Methods to evolve the coordinates of configurations.

[AtomShake](docs/userguide/modules/atomshake/) - Perform Monte Carlo on individual atoms.

[IntraShake](docs/userguide/modules/intrashake/) - Perform Monte Carlo on intramolecular terms.

[MD](modules/md/index) - Run a short molecular dynamics simulation.

[MolShake](modules/molshake/index) - Perform Monte Carlo on whole molecules.

### Forcefield - Energy evaluation, force calculation, and forcefield modification.

[Energy](modules/energy/index) - Calculate total system energy and its contributions.

[EPSR](modules/EPSR) - Refine interatomic potentials according to the methodology of Soper.

[Forces](modules/forces/index) - Calculate atomic forces for configurations.

### Input & Output - Import and export various data, including configuration coordinates and trajectories.

[Export](modules/export/index) - Export various quantities to file (including configurations).

[Import](modules/import/index) - Import data to simulation (e.g. trajectories).

### Optimisation - Minimisation and fitting modules.

[Calibrate](modules/calibrate/index) - Adjust various quantities automatically.

[GeometryOptimisation](modules/geometryoptimisation/index) - Run an energy minimisation in order to optimise geometry.

