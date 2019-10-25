---
title: Home
nav_order: 1
permalink: /
---
# Dissolve

**Structure Refinement for Scattering Data**

&copy; Dr Tristan Youngs 2012-2019

### Contributors
- Simona (2019-)

_Current Version: 0.4.3, Wednesday 12th June 2019_

Dissolve is a classical simulation code intended to help in analysing experimental scattering (diffraction) measurements through generating representative atomic configurations consistent with supplied reference data. Its primary use is for disordered systems such as those comprised of (or containing) liquids and glasses, but it can be applied to much more complex systems.

Check out [Dissolve on GitHub](https://www.github.com/trisyoungs/dissolve) and [contribute](developers/contributing.md) if you want to!

## Getting Started

Basic overview and referencing, and obtaining, installing, and building Dissolve.

[What is / isn't Dissolve?](start/overview.md){: .btn}
[Where to get Dissolve](start/get.md){: .btn}
[Building Dissolve](start/build.md){: .btn}
[Learning Dissolve](start/learn.md){: .btn}
[Referencing Dissolve](start/referencing.md){: .btn}

* * *
## Setting Up Dissolve

[Building a simulation from scratch](setup/scratch.md){: .btn}
[Using a system template](setup/templates.md){: .btn}

* * *
## Running Dissolve

How to run Dissolve, ths input it expects, and a full list of command-line options.

[CLI Options](run/cli.md){: .btn}

* * *
## Input / Output

Dissolve's file format and syntax, keyword descriptions, and output file roles.

TODO
{: .label .label-red }
[Main Input File](io/input/index.md){: .btn .btn-blue}
[Configuration Block](io/input/configuration.md){: .btn .btn-outline}
[Layer Block](io/input/layer.md){: .btn .btn-outline}
[Master Block](io/input/master.md){: .btn .btn-outline}
[Module Block](io/input/module.md){: .btn .btn-outline}
[PairPotential Block](io/input/pairpotential.md){: .btn .btn-outline}
[Simulation Block](io/input/simulation.md){: .btn .btn-outline}
[Site Block](io/input/site.md){: .btn .btn-outline}
[Species Block](io/input/species.md){: .btn .btn-outline}

[Heartbeat File](io/heartbeat.md){: .btn .btn-blue}
[Restart File](io/restart.md){: .btn .btn-blue}

* * *
## Modules

All of Dissolve's modules - their purpose, options, and inner workings.

TODO
{: .label .label-red }

[Overview](modules/index/index.md){: .btn .btn-blue}

### Analysis
Calculation of properties of interest from configurations.

#### General
[Analyse](modules/analyse/index.md){: .btn .btn-outline}

#### Two-Centre Correlation Functions
[CalculateRDF](modules/calculaterdf/index.md){: .btn .btn-outline}

#### Three-Centre Correlation Functions
[CalculateDAngle](modules/calculatedangle/index.md){: .btn .btn-outline}

### Checks & Tests
Modules for checking, testing, and ensuring consistency.

[Checks](modules/checks/index.md){: .btn .btn-outline}
[DataTest](modules/datatest/index.md){: .btn .btn-outline}
[SanityCheck](modules/sanitycheck/index.md){: .btn .btn-outline}

### Correlation Functions
Methods to calculate real- and reciprocal-space correlation functions.

[NeutronSQ](modules/neutronsq/index.md){: .btn .btn-outline}
[RDF](modules/rdf/index.md){: .btn .btn-outline}
[SQ](modules/sq/index.md){: .btn .btn-outline}

### Evolution
Methods to evolve the coordinates of configurations.

[AtomShake](modules/atomshake/index.md){: .btn .btn-outline}
[IntraShake](modules/intrashake/index.md){: .btn .btn-outline}
[MD](modules/md/index.md){: .btn .btn-outline}
[MolShake](modules/molshake/index.md){: .btn .btn-outline}

### Forcefield
Energy evaluation, force calculation, and forcefield modification.

[Energy](modules/energy/index.md){: .btn .btn-outline}
[EPSR](modules/EPSR.md){: .btn .btn-outline}
[Forces](modules/forces/index.md){: .btn .btn-outline}
[Refine](modules/refine/index.md){: .btn .btn-outline}

### Input & Output
Import and export various data, including configuration coordinates and trajectories.

[Export](modules/export/index.md){: .btn .btn-outline}
[Import](modules/import/index.md){: .btn .btn-outline}

### Optimisation
Minimisation and fitting modules.

[Calibrate](modules/calibrate/index.md){: .btn .btn-outline}
[GeometryOptimisation](modules/geometryoptimisation/index.md){: .btn .btn-outline}

* * *
## Developer Documentation

### Definition, Style, and Scope
Architecture and style guidelines, specifications, and workflow descriptions.

[Project Description](developers/description.md){: .btn}
[Project Overview](developers/overview.md){: .btn}
[Coding Standards](developers/standards.md){: .btn}
[Technical Specification](developers/techspec.md){: .btn}
[Architecture](developers/architecture.md){: .btn}
[Contributing](developers/contributing.md){: .btn}
[Releases](developers/releases.md){: .btn}


### Developer Guides

Overviews of classes and implementations relevant to core Dissolve methodology.

[Generic Data Management and Storage](developers/overviews/datamanagement.md){: .btn}
[Developing Modules](developers/overviews/modules.md){: .btn}
[Keyword Options](developers/overviews/keywordoptions.md){: .btn}

TODO
{: .label .label-red }
[Data Visualisers (GUI)](developers/overviews/dataviewers.md){: .btn}
[Aspects of the GUI](developers/overviews/gui.md){: .btn}
[Understanding Configurations](developers/overviews/configurations.md){: .btn}
[Dissolve's Template Classes](developers/overviews/templates.md){: .btn}

* * *
## Project Governance

How the evolution of Dissolve is managed and governed.

[Project Management Committee](governance/project.md){: .btn}
[Technical Steering Committee](governance/technical.md){: .btn}
