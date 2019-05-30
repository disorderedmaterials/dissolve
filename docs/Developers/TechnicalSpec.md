---
parent: Developer's Handbook
nav_order: 4
---
# Technical Specification

## Introduction

### Scope
This document lists the core technical specifications for the Dissolve project. It outlines general and context-specific requirements for the code, and is recommended reading for users and developers of the code.

## Background

See the [project description](ProjectDescription.md) for an overview of the purpose and intention of Dissolve.

## General Requirements

### Codebase

- The source code should be written in a single language which has wide support and good performance (C++).
- The source code should be platform-independent and compilable with freely-available tools.
- The code should be as self-contained as is practically possible, requiring minimal external dependencies (except where significant functionality is gained by utilising a third-party library).

### Technical

- The code should be able to utilise multi-core hardware, spanning single-cpu enclosures up to massively-parallel compute clusters through the use of appropriate parallelisation methodology.
- The code should be memory-efficient such as to allow the simulation of systems containing millions of particles on a mid-range desktop (4 core, 8 Gb memory).
- The code should make use of human-readable, non-proprietary file formats for input/output wherever practically possible. Where a non-proprietary file format is required (e.g. in program-specific outputs, for instance) the file-format should be text-based so as to ensure portability between machine architectures.

### Context-Specific (Baseline)

1. Provide the ability to simulate large and complex systems studied by total scattering in line with the demands of the NIMROD/SANDALS user programme. This should encompass the ability to perform refinement of atomic systems of millions of atoms, in reasonable timescales, as well as those systems considered ‘complex’ and which are not treatable by single state points / configurations (e.g. porous systems where the pores are well-defined geometrically, but span some range of pore radii).
2. Provide an intuitive and easy-to-use set of tools for the setup, running, and analysis of the code for a given problem. The potential user base cannot be assumed to be experts in, or even familiar with molecular simulation and command-line tools, and so a next-generation code should be usable by all skill tiers of the user community without compromising the demands of any particular user level. . Where  possible, the code should aim to limit additional learning for those already familiar with the existing code base (EPSR).
3. Mimic, to the best degree possible within the framework of the new code, the functionality of the existing EPSR software, so as to cover the needs and expectations of the existing user community.
4. Provide alternative routes, beyond those found within EPSR, for the fitting and interrogation of experimental data. This may encompass alternative fitting procedures (e.g. Simplex minimisation of the interaction parameter space, either through standard functional forms or Spline-based potential representations), the ability to interactively adjust interactions between molecule types / functional groups (e.g. sandbox-style approach where the user strengthen / weaken interactions in the system through chemical intuition to see the effects on the structure), and alternative mechanisms of simulation (e.g. molecular dynamics or Grand-Canonical Monte Carlo).

### Context-Specific

1. 
TODO
