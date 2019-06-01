---
title: Modules
parent: Overviews
grand_parent: Developer Documentation
mermaid: true
---
# Modules

## Introduction

### Purpose
This document gives an overview of modules, the core providers of specific simulation / calculation functionality within Dissolve.

### Scope
This overview provides architecture and implementation details for modules within the Dissolve code, and is essential reading for any developer wishing to create a new module for inclusion.

## Overview

### Brief

The critical (read 'interesting') functionality of Dissolve is contained within independent modules. This functionality tends to focus on modifying the contents of, or calculating properties from, one or more configurations. In the latter case, this data may be consumed primarily by either the code itself (i.e. used by other modules) or the user (i.e. graphed to illustrate calculated quantities of interest). Because Dissolve aims to be flexible in its approach, writing specific functionality into independent objects aids in designing and constructing simulations to achieve specific tasks. For more information on Dissolve's main workflow in this regard, see the [architecture](../Architecture.md) document. 

A module can be used multiple times within the same simulation, typically targetting different configurations or utilising different sets of control variables / options. Modules can store and retrieve data they create or need from a suitable `GenericList` (see [data management](DataManagement.md) document) but otherwise have no context of other modules, their position in a `ModuleLayer`, or the overall purpose of the simulation. In this sense they operate as independent processing steps or operations in a linear analysis pipeline.

### What Should a Module Do?

A given module targets zero or more configurations, with zero or more options controlling its operation. A module is permitted to alter the contents of the configuration (e.g. change atomic positions) and may store data in a target `GenericList`, or retrieve existing data to utilise in its processing. Aa module should do a self-contained, single task.

It is important to write modules that assume as little as possible about the state of data outside of its processing function. The contents of configurations and data stored in `GenericList`s should be considered volatile and liable to change signifcantly in between executions of modules. As such, storing data determined from a given configuration, for example, to use in subsequent iterations is a dangerous practice and should be avoided. While this creates obvious overhead in the re-calculation of certain quantities, the penalty in execution time of the module is likely to be small compared to the overall time the module runs for. As such, this is the preferred approach.

### Location / Structure

All modules are located in the `src/modules` directory, with a separate subdirectory (named after the module) for each. For a given module the basic directory and file structure is as follows:

```
module
├── core.cpp
├── definition.cpp
├── functions.cpp
├── nogui.cpp
├── options.cpp
├── process.cpp
├── module.h
└── gui
    ├── gui.cpp
    ├── modulewidget_funcs.cpp
    ├── modulewidget.h
    └── modulewidget.ui
```

It is strongly recommended to maintain this file structure as far as is possible for consistency between modules. The exception to this rule is when the number / size / heterogeneity of auxiliary functions stored in `functions.cpp` exceeds a manageable limit. In this case, partitioning those functions into separate source files is acceptable, although the prefix used for these files should reflect that of the original (e.g. `functions_test.cpp`, `functions_save.cpp`, etc.).

## Implementation

### Relevant Classes

- [Module](https://github.com/trisyoungs/dissolve/tree/develop/src/module/module.h)
- [ModuleLayer](https://github.com/trisyoungs/dissolve/tree/develop/src/module/layer.h)

### Class Diagram

_Note: Only virtual and pure virtual functions are listed, for the purpose of discussion._

  Module : virtual bool setUp(Dissolve& dissolve, ProcessPool& procPool)
  Module : pure virtual bool process(Dissolve& dissolve, ProcessPool& procPool)
```


## Module Definition

The `Module` class is abstract, and is the principal class from which to derive when defining a new module.

### Type, Category, and Targets (`definition.cpp`)

Implements functions returning the type (i.e. general name) of the module (`const char* Module::type()`), a short description of its purpose (`const char* Module::brief()`), the name of the category it should be assigned to (`const char* Module::category()`), mainly for the purposes of grouping within the GUI, and the number of configurations the module can target (`int Module::nTargetableConfigurations()`). The latter can be zero to mean that the module cannot target configurations (in which case an error will be raised if this is attempted), a positive integer to state a fixed number that can / must be targeted, or -1 to indicate that any number can be targeted (even zero).

### Options (`options.cpp`)

Controlling the behaviour of the module is achieved by defining a set of keyword options in `Module::setUpKeywords()`. These keywords are given a name by which they are referenced, a type with a default value, and a description. Each POD as well as a multitude of compound objects useful to Dissolve are represented by classes derived from `ModuleKeyword`, which take care of the storage, reading, writing, and limit / validation checking. In addition, each has its own associated UI representation class which is generated automatically when required, meaning that no additional effort is required to provide controls for modules within the GUI. See the document on [keyword options](KeywordOptions.md) for a more in-depth discussion and a list of available keyword classes.

`Module::setUpKeywords()` should also be used to set the execution `frequency_` if it is required to do so.

### Auxiliary Functions (`functions.cpp`)

Contains auxiliary functions for the module, typically called from the main `Module::process()` function.

### Core Class Functions (`core.cpp`)

Contains the constructor and destructor, and implements the new instance return function (`Module* Module::createInstance()`) which simply returns a `new` instance of the derived class. There is usually no need to modify this file when implementing a new module.


TODO Options (esp. link to GUI)
## Steps Implementing New Modules
