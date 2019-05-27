# Data Management

## Introduction

### Purpose
This document gives an overview of XXX.

### Scope
This overview provides implementation details on the XXX used with the Dissolve code, presenting the salient design decisions, architectures, and classes relevant to its implementation. The document is suitable for developers wishing to understand, modify, fix, or otherwise extend the code.

### Definitions


## Overview

### Brief

The core functionality of Dissolve is contained within independent (modules)[overviews/Modules.md], which may run in any order. Each module typically creates useful data that are of interest to the user (e.g. calculations which yield some specific property of interest), to other modules (which take pre-existing data and use it as the basis for other calculations), or which is necessary to store in order to permit a successful restart of the simulation (i.e. historic data used when performing averaging). These data may be PODs, simple classes, or templated objects.

### Rationale

Storage of these data locally within the modules itself and providing standard accessors is undesirable since this requires that modules requiring data stored in another module must know the address and relevant member accessor function to retrieve it, increasing the interface complexity as well as the fragility between (strictly independent) modules. Instead, data are stored in a type-agnostic container class, tagged with a descriptive name in order to permit retrieval. These containers are used as centralised repositories (located in the main Dissolve class or locally in configurations) for data created during the main simulation loop

This 'blackboard-style' approach to the storage of data has the following benefits:
1. Since the sequence of modules to be executed is not known _a priori_, the lack of availabilty of a named piece of data is meaningful, and eliminates the need to check for specific module types having already been executed and their data prepared for others to use.
1. Modules may implement different algorithms in order to calculate the same (named) data, and which can then be accessed by other modules without requiring knowledge of the creating module.
1. Data relevant to the restarting / continuation of simulations is contained in a single area outside of any modules, greatly simplifying the creation and loading of restart files.




### Implementation

The primary storage class for Dissolve's type-agnostic data is the GenericList 
