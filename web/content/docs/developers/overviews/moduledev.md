---
title: Modules
---

## Introduction

### Purpose
This document gives an overview of modules, the core providers of specific simulation / calculation functionality within Dissolve.

### Scope
This overview provides architecture and implementation details for modules within the Dissolve code, and is essential reading for any developer wishing to create a new module for inclusion.

## Overview

### Brief

The critical (read 'interesting') functionality of Dissolve is contained within independent modules. This functionality tends to focus on modifying the contents of, or calculating properties from, one or more configurations. In the latter case, this data may be consumed primarily by either the code itself (i.e. used by other modules) or the user (i.e. graphed to illustrate calculated quantities of interest). Because Dissolve aims to be flexible in its approach, writing specific functionality into independent objects aids in designing and constructing simulations to achieve specific tasks. For more information on Dissolve's main workflow in this regard, see the [architecture](../../architecture.md) document.

A module can be used multiple times within the same simulation, typically targetting different configurations or utilising different sets of control variables / options. Modules can store and retrieve data they create or need from a suitable `GenericList` (see the [data management](../datamanagement) document) but otherwise have no context of other modules, their position in a `ModuleLayer`, or the overall purpose of the simulation. In this sense they operate as independent processing steps or operations in a linear analysis pipeline.

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

- [Module](https://github.com/projectdissolve/dissolve/tree/develop/src/module/module.h)
- [ModuleLayer](https://github.com/projectdissolve/dissolve/tree/develop/src/module/layer.h)

### Class Diagram

_Note: For brevity on only the key (protected) member variables and virtual functions are listed._

{{< mermaid >}}
classDiagram
    Module --> ModuleKeywordList
    Module --> Configuration
    class Module {
        # int frequency_
        # bool enabled_
        # bool configurationLocal_
        # RefList~Configuration,bool~ targetConfigurations_
        + createInstance()* Module
        + type()* const char
        + brief()* const char
        + category()* const char
        + nTargetableConfigurations()* int
        # initialise()* void
        + setUp(Dissolve &dissolve, ProcessPool &procPool) bool
        - process(Dissolve &dissolve, ProcessPool &procPool)* bool
        + createWidget(QWidget *parent, Dissolve &dissolve) ModuleWidget
    }
{{< /mermaid >}}

## Module Definition

The `Module` class is abstract, and is the principal class from which to derive when defining a new module.

### Type, Category, and Targets (`definition.cpp`)

Implements functions returning the type (i.e. general name) of the module (`const char *Module::type()`), a short description of its purpose (`const char *Module::brief()`), the name of the category it should be assigned to (`const char *Module::category()`), mainly for the purposes of grouping within the GUI, and the number of configurations the module can target (`int Module::nTargetableConfigurations()`). The latter can be zero to mean that the module cannot target configurations (in which case an error will be raised if this is attempted), a positive integer to state a fixed number that can / must be targeted, or -1 to indicate that any number can be targeted (even zero).

### Initialisation (`init.cpp`)

The `Module::initialise()` function is used to set up any complex data within the Module and, critically, any keywords that control the behaviour of the module. These keywords are given a name by which they are referenced, a type with a default value, and a description. Each POD as well as a multitude of compound objects useful to Dissolve are represented by classes derived from `ModuleKeyword`, which take care of the storage, reading, writing, and limit / validation checking. In addition, each has its own associated UI representation class which is generated automatically when required, meaning that no additional effort is required to provide controls for modules within the GUI. See the document on [keyword options](../keywordoptions.md) for a more in-depth discussion and a list of available keyword classes.

Note that keywords are constructed for each instance of the module, and so are independent between modules of the same type.

### Set-Up and Processing (`process.cpp`)

Each module has the opportunity to set-up any necessary data prior to it being run through the `Module::setUp(Dissolve &dissolve, ProcessPool &procPool)` virtual. For each module instance the function is called only once as one of the final acts of [`Dissolve::setUpSimulation()`](https://github.com/projectdissolve/dissolve/tree/develop/src/main/dissolve.cpp#L276). References to the master `Dissolve` and current `ProcessPool` are provided to permit full access to necessary data and allow master-only processing and/or distribution of any data.

Useful work is performed by `Module::process(Dissolve &dissolve, ProcessPool &procPool)` and which should contain the implementation of the desired algorithm, calculation, or processing to be performed. The function itself is private and only called by the public `Module::executeProcessing()` method, which also obtains and stores suitable timing information. Often it is not practical to perform the whole processing in one linear function call, and so any required additional functions may be implemented in `functions.cpp`.

### Auxiliary Functions (`functions.cpp`)

Contains auxiliary functions for the module, typically called from the main `Module::process()` function.

### Core Class Functions (`core.cpp`)

Contains the constructor and destructor, and implements the new instance return function (`Module *Module::createInstance()`) which simply returns a `new` instance of the derived class. There is usually no need to modify this file when implementing a new module.

### Dummy GUI Creation (`nogui.cpp`)

Although a default implementation for the `createWidget(QWidget *parent, Dissolve &dissolve)` virtual is provided by `Module` (and which returns `NULL`), when a GUI **is** required for the module it is necessary to reimplement the same function in `nogui.cpp`. This is to allow a proper GUI widget to be created and returned by the implementation found in the module's `gui/gui.cpp`, but still allow the module to be built cleanlyi without a GUI (or any associated dependencies) when required. As such, the `nogui.cpp` implementation of the function is used when a GUI is present, but is not being built.

Note that, since the necessary `QWidget` is declared as a pointer and is forward-declared in `src/module/module.h' the modules can be built cleanly and without introducing dependencies on the Qt libraries when required.

### GUI Widget (`gui/`)

XXX TODO

## Steps for Implementing New Modules

### 1) Copy Module Template

Create a copy of an existing module and rename its classes accordingly to avoid clashes. A [`SkeletonModule`](https://github.com/projectdissolve/dissolve/tree/develop/src/modules/skeleton) providing the basic file structure and build files is included in the source, and may be used as a fresh starting point. On Linux a [shell script](https://github.com/projectdissolve/dissolve/tree/develop/src/modules/skeleton/renamemodule) is available to rename the class, files, and header blocks of `SkeletonModule`.

`SkeletonModule` contains files for an associated GUI. If this is **not** required, remove the `gui` subdirectory, delete the `SUBDIRS` target and `libmodulewidget_la_SOURCES += nogui.cpp` line from the module's `Makefile.am`, and remove the 'add_subdirectory(gui)` line from the module's `CMakeLists.txt`. The `nogui.cpp` source file can also be safely deleted.

### 2) Update CMake

Next, add the new module to the CMake configuration. Assuming the new module is located in `src/modules/mystery`, the module's associated base library name is `mysterymodule`:

- In the root `CMakeLists.txt` add the module library target to the `BASIC_LINK_LIBS` variable.
- In the root `CMakeLists.txt` add the 'nogui' module library target (e.g. `mysterymodulenogui`) to the `target_link_libraries( {$target_name} ...` section.
- In the root `CMakeLists.txt` add the 'gui' module library target (e.g. `mysterymodulegui`) to the `target_link_libraries( {$gui_target_name} ...` and `target_link_libraries( {$ed_target_name} ...` sections.
- In `src/modules/CMakeLists.txt`, add the new module's directory as a subdirectory target with the `add_subdirectory` directive.
