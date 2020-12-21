---
title: EPSR (Module)
linkTitle: EPSR
description: Refine interatomic potentials according to the methodology of Soper
---

## Overview

The `EPSR` module implements the core algorithms of the EPSR code written by A. K. Soper, and allows EPSR-like simulations to be run in Dissolve whilst still employing a full classical forcefield. Its purpose is to automatically adjust the current interatomic pair potentials by adding on a tabulated "correction" potential (the titular "empirical" potential) in order to promote agreement of calculated structure factors with experimental data. This is performed by essentially inverting the experimental data and deriving the empirical potentials based on the observed differences with the simulated data.

The `EPSR` module reproduces the core workflow of the original EPSR, and as such can handle both neutron and x-ray datasets. However, in order to stay faithful to the original code it is restricted to using calculated structure factors based on only a single reference configuration (i.e. multi-configuration fits are not possible).

## Description

### Basic Theory

What follows is a brief overview of the basic ideas behind the EPSR methodology. For a more in-depth explanation see the [EPSR manual](https://www.isis.stfc.ac.uk/OtherFiles/Disordered%20Materials/EPSR25%20Manual%202017-10.pdf) itself, or for an abridged version try the [Dissolve paper](https://www.tandfonline.com/doi/abs/10.1080/00268976.2019.1651918).

{{< todo-label >}}

### Intramolecular Potentials

## Configuration

### Control Keywords
|Keyword|Arguments|Default|Description|
|:------|:-------:|:-----:|-----------|
|`StabilityThreshold`|`double`|`1.0e-3`|Threshold value at which energy is deemed stable over the defined windowing period|
|`StabilityWindow`|`int`|`10`|Number of points over which to assess the stability of the energy (per Configuration)|
