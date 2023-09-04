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

## Options

### Targets

|Keyword|Arguments|Default|Description|
|:------|:-------:|:-----:|-----------|
|`Target`|`Module ...`|--|List of {{< module "NeutronSQ" >}} and/or {{< module "XRaySQ" >}} modules from which to take reference data and introduce into the fitting procedure|
|`TargetWeight`|`Module`<br/>`double`|--|The name of a target module and the weighting it should have in the scattering matrix. If not specified a target module has a standard weighting of `1.0`.|

### Control

|Keyword|Arguments|Default|Description|
|:------|:-------:|:-----:|-----------|
|`EReq`|`double`|`3.0`|Limit of magnitude of additional potential for any one pair potential|
|`Feedback`|`double`|`0.9`|Confidence factor|
|`ModifyPotential`|`int`|`1`|Frequency at which to apply generated perturbations to interatomic potentials. A value of `1` modifies the potential every time the module is run, while higher values create delays in the modification (other aspects such as fitting of delta F(Q) are still performed by the module)|
|`QMax`|`double`|`30.0`|Maximum Q value over which to generate potentials from total scattering data|
|`QMin`|`double`|`0.5`|Minimum Q value over which to generate potentials from total scattering data|

## Advanced

### Control

|Keyword|Arguments|Default|Description|
|:------|:-------:|:-----:|-----------|
|`Weighting`|`double`|`1.9`|Factor used when adding fluctuation coefficients to pair potentials|
|`OverwritePotentials`|`bool`|`false`|Overwrite potentials each time rather than summing them|

### Expansion Function

|Keyword|Arguments|Default|Description|
|:------|:-------:|:-----:|-----------|
|`Weighting`|`[ExpansionFunction]({{< ref "expansionfunction" >}})`|`Gaussian`|Form of expansion function to use when fitting difference data|
|`GSigma1`|`double`|`0.1`|Width for Gaussian function in reciprocal space|
|`GSigma2`|`double`|`0.2`|Width for Gaussian function in real space|
|`NCoeffP`|`int`|--|Number of coefficients used to define the empirical potential - this is defined based on the potential range if not provided explicitly|
|`NPItSs`|`int`|`1000`|Number of iterations when refining fits to delta functions|
|`InpAFile`|`string`|--|EPSR inpa file from which to read starting coefficients from|
|`PCofFile`|`string`|--|EPSR pcof file from which to read empirical potential coefficients|
|`PSigma1`|`double`|`0.01`|Width for Poisson functions in reciprocal space (N.B. this is psigma2 in EPSR)|
|`PSigma2`|`double`|`0.01`|Width for Poisson functions in real space|
|`RMaxPT`|`double`|`-1.0`|Radius at which potential truncation goes to zero (-1.0 to use pair potential maximum range)|
|`RMinPT`|`double`|`1.9`|Radius at which potential truncation begins (-1.0 to set to 2.0 Angstroms under rmaxpt)|
|`Smoothing`|`int`|`0`|Smoothing to apply to fluctuation coefficients before summation into potential|

## Export

|Keyword|Arguments|Default|Description|
|:------|:-------:|:-----:|-----------|
|`SaveDifferenceFunctions`|`bool`|`false`|Whether to save difference function and fit|
|`SaveEmpiricalPotentials`|`bool`|`false`|Whether to save empirical potentials|
|`SaveEstimatedPartials`|`bool`|`false`|Whether to save estimated partials|
|`SavePCof`|`bool`|`false`|Whether to save potential coefficients|
|`SaveSimulatedFR`|`bool`|`false`|Whether to save simulated F(r) (Fourier transform of calculated F(Q))|
