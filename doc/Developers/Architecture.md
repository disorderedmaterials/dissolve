# Architecture

## Introduction

### Purpose
This document outlines the basic operation of the Dissolve code, covering initialisation and processing steps.

### Scope
This document introduces the main stages of operation of Dissolve, describing each step in detail, and is primarily useful for developers and contributors who wish to understand the basic workflow utilised by the code.

## Definitions

CONFIGURATIOn
MODULE
SIMULATIOn
## Overview


Dissolve stores the definitions for, and operates on, one or more _configurations_. A _configuration_ is a representation of an experimentally-measured system, comprising atoms at specific positions, the chemical bonds between them, and the molecular units that they represent. Since the nature, size, and content of the experimentally-measured systems is by nature extremely varied, Dissolve does not assume any default analysis workflow or path. Instead, specific operations / algorithms / calculations are implemented as separate _modules_, and which are assembled into a workflow on a case-by-case basis, specified by the user, in order to best analyse the current system under study.

The main Dissolve class, therefore, maintains a list of current configurations, a list of available modules, and one or more sequences of modules to run during its main loop.

