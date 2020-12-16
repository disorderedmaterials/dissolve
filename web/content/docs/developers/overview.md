---
title: Project Overview
description: Light overview of the codebase and its organisation
---

## Introduction

### Purpose
This document provides a high-level overview of the structure of the Dissolve project, its language and associated build systems, and source tree layout.

### Scope
This overview provides basic information suitable for developers wishing to build and contribute to the project, spanning on the coding language, executable targets, build systems, external dependencies and source tree layout.

## Basics

### Project Purpose
Dissolve is a code to permit interrogation of scattering data through the classical simulation of underlying experimental systems. For more of an overview see the [project description]({{< ref "description" >}}) document.

### Language
Dissolve is written entirely in C++. For relevant conventions and guidelines regarding style see the [coding standards]({{< ref "standards" >}}) document.

## Build System

The CMake build sytem is used in provide cross-platform compatibility.

### Executable Targets
- `dissolve`: Serial CLI version.
- `dissolve-mpi`: (Optional) Parallel CLI version (requires suitable MPI compiler).
- `dissolve-gui`: (Optional) Graphical user interface for Dissolve.

Note that binaries are named as listed above only on Linux and OSX. Windows binaries are built as `Dissolve`, `Dissolve-MPI`, and `Dissolve-GUI`.

### External Dependencies
Dissolve has a modest set of external dependencies, as detailed in the [compilation]({{< ref "compilation" >}}) instructions.

## Source Tree Layout
All source files are contained within the `src/` directory which is laid out as follows:

### Files
- `dissolve-gui.cpp`: Main entry point for GUI.
- `main.cpp`: Main entry point for serial and parallel versions (determined via the `PARALLEL` define).
- `version.h`: Header containing version information.

### Sub-directories
- `analyse`: Core classes used in the piece-wise construction of calculation pathways in relevant analysis modules.
- `base`: Classes providing general functionality that is free of context from Dissolve and its intended application.
- `classes`: Classes providing core contextual objects used in the operation of Dissolve, and which depend only on other classes in the same directory or those in `base/`.
- `data`: Objects providing static data such as the periodic table, scattering lengths, forcefield parameters etc.
- `expression`: Mathematical expression parser / generator.
- `genericitems`: Classes permitting storage of data in a type-agnostic way in order to enable suitable [data management]({{< ref "datamanagement" >}}).
- `gui`: Qt-based GUI for Dissolve.
- `main`: Main class providing Dissolve's core object.
- `math`: Math-related classes, including classes containing (static) data operations and methods.
- `module`: Base definition and associated helper classes for defining and handling [modules]({{< ref "moduledev" >}}).
- `modules`: Repository containing subdirectories defining individual modules.
- `templates`: Template classes providing general, context-free functionality.
