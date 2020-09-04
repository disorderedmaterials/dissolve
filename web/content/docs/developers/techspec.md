---
title: Technical Specifications
description: General technical requirements for the project
---

## Introduction

### Purpose
This document lays out the general technical requirements for the Dissolve project.

### Scope
This document lists the high-level technical specifications for the Dissolve project. It outlines general and context-specific requirements for the code, and is recommended reading for users and developers of the code.

## Background

See the [project description](../description) for an overview of the purpose and intention of Dissolve.

## General Requirements

### Codebase

- The source code should be written in a single language which has wide support and good performance (C++).
- The source code should be platform-independent and compilable with freely-available tools.
- The code should be as self-contained as is practically possible, requiring minimal external dependencies (except where significant functionality is gained by utilising a third-party library).

### Technical

- The code should be able to utilise multi-core hardware, spanning single-cpu enclosures up to massively-parallel compute clusters through the use of appropriate parallelisation methodology.
- The code should be memory-efficient such as to allow the simulation of systems containing millions of particles on a mid-range desktop (4 core, 8 Gb memory).
- Output files written by the code should be text-based so as to ensure portability between machine architectures.

### End-User
- The code must provide a GUI suitable for the setting up, monitoring, and running of simulations. This includes graphing of calculated data.
- The code should be flexible enough to be able to deal with a wide variety of different systems and use-cases, offering the capability to easily add / remove steps into the general execution pipeline.
- The code should provide a degree of extensibility by users in order to permit the addition of new algorithms, calculation types etc.
- The code should make use of human-readable, non-proprietary file formats for input wherever practically possible, so as to make setup and debugging as straightforward as possible.

### Consistency
- The code should implement core algorithms of the existing Empirical Potential Structure Refinement (EPSR) code so as to provide smooth transition in the user community.

