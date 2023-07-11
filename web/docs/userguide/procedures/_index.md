---
title: 8. Procedures
description: Defining procedures to create, analyse, and edit
weight: 8
---

## Overview

Procedures are used within Dissolve to allow user-defined tasks to be created and used for various jobs - e.g. generating the contents for a configuration, performing a specific analysis etc. They are simple sequences of steps or "nodes" that are run when required, with each node having a focussed and specific purpose. Combined, the actions of these simplistic nodes can generate and manipulate complex coordinate ensembles, and analyse complicated structural patterns.

There are three stages to any procedure and its constituent nodes, with each stage performed every time the procedure is run:

#### Set Up
Before the procedure is executed, every node present within it is permitted a set-up stage during which any critical values can be retrieved for use, or any data structures (e.g. to hold results) can be initialised.

#### Execute
The main work of the procedure is performed within the execution stages of the nodes, which are run according to the structure and sequence defined in the procedure.

#### Finalise
Once the procedure has completed its execution stage, every node present within it is permitted a final operation during which data can be retrieved, normalised, processed, saved etc.
, or printing out a final summary of the node's work over the entire procedure.

## Nodes

A procedure is made up of one or more "nodes", each of which performs a specific task on or with some defined objects in a simulation (e.g. species or configuration boxes), generates data from the simulation, or manipulates data generated by other nodes.

## Branches

The flow of operation between nodes is completely linear, but some nodes have branches containing other sequences of nodes (and further branches). These branches may be executed every time the node runs, or may only run under certain conditions. See the individual node reference pages for details.

### Control Options

Each node has a number of control keywords that define how the node will act.

### Value Expressions

For flexibility, node options that require numerical values understand [mathematical expressions]({{< ref "expressions" >}}) rather than just plain integers or floating-point values. They can also reference parameters from other nodes, such as those defined in a [`ParametersNode`]({{< ref "parametersnode" >}}).