---
title: Home
nav_order: 1
permalink: /
---
# Dissolve

**Structure Refinement for Scattering Data**

&copy; Dr Tristan Youngs 2012-2019

_Current Version: 0.4.0, 30th May 2019_

Dissolve is a classical simulation code intended to help in analysing experimental scattering (diffraction) measurements through generating representative atomic configurations consistent with supplied reference data. Its primary use is for disordered systems such as those comprised of (or containing) liquids and glasses, but it can be applied to much more complex systems.

## Introduction

[What is / isn't Dissolve?](Introduction/WhatIsDissolve.md){: .btn}

* * *
## Input / Output

Dissolve's file format and syntax, and output file roles.

TODO
{: .label .label-red }
[Overview](Files/InputFile/Index.md){: .btn .btn-blue}
[Configuration Block](Files/InputFile/Configuration.md){: .btn .btn-outline}
[Layer Block](Files/InputFile/Layer.md){: .btn .btn-outline}
[Master Block](Files/InputFile/Master.md){: .btn .btn-outline}
[Module Block](Files/InputFile/Module.md){: .btn .btn-outline}
[PairPotential Block](Files/InputFile/PairPotential.md){: .btn .btn-outline}
[Simulation Block](Files/InputFile/Simulation.md){: .btn .btn-outline}
[Site Block](Files/InputFile/Site.md){: .btn .btn-outline}
[Species Block](Files/InputFile/Species.md){: .btn .btn-outline}
[SpeciesInfo Block](Files/InputFile/SpeciesInfo.md){: .btn .btn-outline}

[Heartbeat File](Files/Heartbeat.md){: .btn .btn-blue}
[Restart File](Files/Restart.md){: .btn .btn-blue}

* * *
## Modules

All of Dissolve's modules - their purpose, options, and inner workings.

TODO
{: .label .label-red }

[Overview](Modules/Index.md){: .btn .btn-blue}

### Analysis
Calculation of properties of interest from configurations.

#### General
[Analyse](Modules/Analyse.md){: .btn .btn-outline}

#### Two-Body Correlation Functions
[CalculateRDF](Modules/CalculateRDF.md){: .btn .btn-outline}

### Checks & Tests
Modules for checking, testing, and ensuring consistency.

[Checks](Modules/Checks.md){: .btn .btn-outline}
[DataTest](Modules/DataTest.md){: .btn .btn-outline}
[SanityCheck](Modules/SanityCheck.md){: .btn .btn-outline}

### Correlation Functions
Methods to calculate real- and reciprocal-space correlation functions.

[NeutronSQ](Modules/NeutronSQ.md){: .btn .btn-outline}
[RDF](Modules/RDF.md){: .btn .btn-outline}
[SQ](Modules/SQ.md){: .btn .btn-outline}

### Evolution
Methods to evolve the coordinates of configurations.

[AtomShake](Modules/AtomShake.md){: .btn .btn-outline}
[IntraShake](Modules/IntraShake.md){: .btn .btn-outline}
[MD](Modules/MD.md){: .btn .btn-outline}
[MolShake](Modules/MolShake.md){: .btn .btn-outline}

### Forcefield
Energy evaluation, force calculation, and forcefield modification.

[Energy](Modules/Energy.md){: .btn .btn-outline}
[EPSR](Modules/EPSR.md){: .btn .btn-outline}
[Forces](Modules/Forces.md){: .btn .btn-outline}
[Refine](Modules/Refine.md){: .btn .btn-outline}

### Input & Output
Import and export various data, including configuration coordinates and trajectories.

[Export](Modules/Export.md){: .btn .btn-outline}
[Import](Modules/Import.md){: .btn .btn-outline}

### Optimisation
Minimisation and fitting modules.

[Calibrate](Modules/Calibrate.md){: .btn .btn-outline}
[GeometryOptimisation](Modules/GeometryOptimisation.md){: .btn .btn-outline}

* * *
## Developer Documentation

### Definition, Style, and Scope
Architecture and style guidelines, specifications, and workflow descriptions.

[Project Description](Developers/Description.md){: .btn}
[Project Overview](Developers/ProjectOverview.md){: .btn}
[Coding Standards](Developers/CodingStandards.md){: .btn}
[Technical Specification](Developers/TechnicalSpec.md){: .btn}
[Architecture](Developers/Architecture.md){: .btn}
[Workflows](Developers/Workflows.md){: .btn}


### Developer Guides

Overviews of classes and implementations relevant to core Dissolve methodology.

[Generic Data Management and Storage](Developers/Overviews/DataManagement.md){: .btn}
[Developing Modules](Developers/Overviews/Modules.md){: .btn}
[Keyword Options](Developers/Overviews/KeywordOptions.md){: .btn}

TODO
{: .label .label-red }
[Data Visualisers (GUI)](Developers/Overviews/DataViewers.md){: .btn}
[Aspects of the GUI](Developers/Overviews/GUI.md){: .btn}
[Understanding Configurations](Developers/Overviews/Configurations.md){: .btn}
[Dissolve's Template Classes](Developers/Overviews/Templates.md){: .btn}

* * *
## Project Governance

How the evolution of Dissolve is managed and governed.

[Project Management Committee](Governance/ProjectManagementCommittee.md){: .btn}
[Technical Steering Committee](Governance/TechnicalSteeringCommittee.md){: .btn}
