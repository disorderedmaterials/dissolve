---
title: Modules
parent: Overviews
grand_parent: Developer Documentation
---
# Modules

## Introduction

### Purpose
This document gives an overview of modules, the core providers of specific simulation / calculation functionality within Dissolve.

### Scope
This overview provides architecture and implementation details for modules within the Dissolve code, and is essential reading for any developer wishing to create a new module for inclusion.

## Overview

### Brief

The critical (read 'interesting') functionality of Dissolve is contained within independent modules. This functionality tends to focus on modifying the contents of, or calculating properties from, one or more configurations. In the latter case, this data may be consumed primarily by either the code itself (i.e. used by other modules) or the user (i.e. graphed to illustrate calculated quantities of interest). Because Dissolve aims to be flexible in its approach, writing specific functionality into independent objects aids in designing and constructing simulations to achieve specific tasks. For more information on Dissolve's main workfloe in this regard, see the [architecture](../Architecture.md) document. 

A module can be used multiple times within the same simulation, typically targetting different configurations or utilising different sets of control variables / options. Modules can store and retrieve data they create or need from a suitable `GenericList` (see [data management](DataManagement.md) document) but otherwise have no context of other modules, their position in a `ModuleLayer`, or the overall purpose of the simulation. In this sense they operate as independent processing steps or operations in a linear analysis pipeline.

## Implementation

### Relevant Classes

- [Module](https://github.com/trisyoungs/dissolve/tree/develop/src/module/module.h)
- [ModuleLayer](https://github.com/trisyoungs/dissolve/tree/develop/src/module/layer.h)

### Details

TODO Module Directory Layout
TODO Options (esp. link to GUI)
TODO Steps Implementing New Modules
