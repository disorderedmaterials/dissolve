---
title: Architecture
description: Basic flow of the Dissolve code
---

## Introduction

### Purpose
This document outlines the basic operation of the Dissolve code, covering initialisation and processing steps.

### Scope
This document introduces the main stages of operation of Dissolve, describing each step in detail, and is primarily useful for developers and contributors who wish to understand the basic workflow utilised by the code.

## Definitions

- _Species_: An atomic, molecular, or other component present in the experimental system
- _Configuration_: A representation of an experimentally-measured system, comprising atoms at specific positions, the chemical bonds between them, and the _species_ that they represent.
- _Module_: A processing object which calculates one or more useful quantities, from zero or more _configurations_.
- _Module Layer_ or simply _Layer_: A collection of _modules_, in a specific order, that when run in sequence result in output useful to the user.
- _Forcefield_: A collection of terms describing the interactions between atoms within and between _species_ within a _configuration_.
- _Simulation_: The entire process that Dissolve performs, utilising _configurations_ of _species_, the _forcefield_, and _layers_ of _modules_.

## Overview

The primary use case of Dissolve is to help interrogate experimental data through the use of various classical simulation techniques. Typically, scattering measurements will have been made on some target system of interest (a liquid, solid, glass etc.) and so experimental structural data describing the system in some way is available. The purpose of Dissolve is to replicate these target data from a supplied set of one or more configurations through the application of classical atomistic simulation techniques (encapsulated within individual modules).

Since the nature, size, and content of the experimentally-measured systems is by nature extremely varied, Dissolve does not assume any default style of simulation to apply. Instead, individual modules (encapsulating specific calculations or algorithms) are assembled in to layers in order to create a suitable simulation and analysis pipeline for the systems under study. There are several general simulation styles that are considered "common usage" and so templates for these types of processing are provided.

## Relevant Classes

- [`Dissolve`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/main/dissolve.h)
- [`CoreData`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/classes/coredata.h)
- [`Configuration`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/classes/configuration.h)
- [`Species`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/classes/species.h)
- [`ModuleLayer`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/module/layer.h)

## Class Dependency

_Note: For brevity only the critical variables and members are shown._

{{< mermaid >}}
classDiagram
    Dissolve-->CoreData
    Dissolve-->Module
    class Dissolve {
        - CoreData coreData_
        - List~Module~ masterModules_
        - RefList~Module,bool~ moduleInstances_
        + registerMasterModules() bool
        + loadInput(const char *filename) bool
        + loadRestart(const char *filename) bool
        + setUp() : bool
        + iterate(int nIterations) : bool
    }

    CoreData-->Species
    CoreData-->ModuleLayer
    CoreData-->Configuration
    class CoreData {
        - List~Species~ species_
        - List~Configuration~ configurations_
        - List~ModuleLayer~ processingLayers_
    }

    ModuleLayer-->Module

    Configuration o-- Species
    class Configuration {
        - List~SpeciesInfo~ usedSpecies_
    }
{{< /mermaid >}}

## Implementation

### Initialisation and Set-Up

The main Dissolve class takes a reference to a [`CoreData`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/classes/coredata.h)) object in its constructor, and which stores configuration, species, and processing layer definitions, as well as some forcefield-related terms. [`Dissolve`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/main/dissolve.h)) does not own the [`CoreData`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/classes/coredata.h)) object itself, since it is useful to able to transfer the [`CoreData`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/classes/coredata.h)) reference through the code, without unnecessary baggage.

Following instantiation of [`CoreData`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/classes/coredata.h)) and [`Dissolve`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/main/dissolve.h)) objects in [`main.cpp`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/main.cpp) the following major set-up steps are then performed:

#### 1. Load Input File

Following module registration the main input file is then loaded. This file contains a complete description of the contents (species and configurations) of the system, its forcefield, and any processing layers to construct and run. This initialises and creates the necessary structures within both [`CoreData`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/classes/coredata.h) and [`Dissolve`](https://github.com/disorderedmaterials/dissolve/tree/develop/src/main/dissolve.h). The input file is free-format and keyword-based, and is [detailed elsewhere]({{< ref "inputfile" >}})). While syntax checking of the input file is obviously performed while reading, further checking of the relationship between defined objects within the simulation is performed later on.

#### 2. Simulation Set-Up

Now that all the required data has been loaded, a final consistency check on the simulation data is performed by `Dissolve::checkSetUp()`. Primarily these checks involve ensuring that the necessary pointers to data or other objects are non-NULL, but some sanity checks on other aspects that would result in a non-physical system are also made.

#### 3. Load Restart File

Having established the core objects (species, configurations etc.) in the simulation, the restart file (if it exists) is then loaded through `Dissolve::loadRestart()`. The restart file contains useful data generated by modules and coordinates of configurations to enable a failed / stopped simulation to be restarted.

### Main Loop

Assuming that no errors have arisen to this point, Dissolve is ready to enter its main loop. The loop runs for a specific number of iterations from zero (if just input file checks are required) to a number specified by the user (or defaulting to 5). The main loop actions are organised as follows:

{{< mermaid >}}
graph TD;
  Start((Iteration N)) --> PrintTasks;
  PrintTasks[Print Task Schedule] --> Heartbeat;
  Heartbeat[Write Heartbeat File] --> ConfigCheck;
  subgraph  ForEach Configuration;
    ConfigCheck{Run This Iteration?} --> |Yes|ConfigRun;
    ConfigRun[Execute Local Module Layer];
  end;
  ConfigCheck --> |No|Reassemble;
  ConfigRun --> Reassemble;
  Reassemble[Reassemble Data] --> ProcCheck;
  subgraph  ForEach Main Processing Layer;
    ProcCheck{Run This Iteration?} --> |Yes|ProcRun;
    ProcRun[Execute Processing Module Layer];
  end;
  ProcCheck --> |No|RestartCheck;
  ProcRun --> RestartCheck;
  subgraph  Write Restart File;
    RestartCheck{Run This Iteration?} --> |Yes|RestartRun;
    RestartRun[Write Restart File];
  end;
  RestartCheck --> |No|Continue;
  RestartRun --> Continue;
  Continue((Next Iteration));
{{< /mermaid >}}

#### 1. Print Task Schedule

At the beginning of the Nth loop iteration a schedule of the execution status of all modules is printed, stating when the modules will run (e.g. this iteration, in M iterations time etc.).

#### 2. Configuration Upkeep

For each defined configuration in the simulation any upkeep tasks are performed ahead of any processing by modules.

#### 3. Main Processing

Each defined module layer defined in the `Dissolve::processingLayers_` vector is now executed, if its frequency permits, running each module in its defined sequence. Note that the modules themselves have an execution frequency relative to the number of executions of the parent layer.

#### 4. Write Restart File

Periodically a [restart file]({{< ref "restart" >}}) is written which contains any relevant data generated by modules, with the intention that this data can be read back in so as to seamlessly restart a finished / terminated prior simulation.
