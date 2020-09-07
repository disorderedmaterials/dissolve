---
title: Description
description: What is is Dissolve, and why does it exist
---

## Introduction

### Purpose
This document lays out the background to the creation of the Dissolve code, its purpose, and its baseline requirements.

### Scope
This document describes and defines the Dissolve project in more detail and in a manner suitable for users, contributors, and developers to review the purpose of the project as well as the background driving its creation.

## Overview

Dissolve is a tool to analyse experimental scattering data through classical atomistic modelling of the underlying systems. It's purpose is to replicate and extend the functionality of an existing code, Empirical Potential Structure Refinement (EPSR). It's primary user base is that of the Disordered Materials group at the ISIS Pulsed Neutron and Muon Source, STFC Rutherford Appleton Laboratory, Harwell Oxford, UK.

Within the Disordered Materials group there is a significant driver and need for the simulation of experimental scattering data, in order to obtain the best from the user programme experiments. While isotopic substitution of target systems provide multiple datasets by which to investigate the structure of disordered systems, for all but the simplest systems it is not possible to perform enough isotopic substitutions in order to uniquely extract all the partial structure factors / radial distribution functions of interest. Molecular simulation of the obtained data is, therefore, key to obtaining the full set of partials in a consistent and realistic manner.

To date, the solution to this problem has been provided in the form of the EPSR code developed over the past 20 years by Prof. A. K. Soper. EPSR has driven (and continues to drive) the scientific output of the Disordered Materials group, through allowing users to probe structural features of their targeted systems that would otherwise not be possible using isotopic substitution alone.  Crucially, it must be recognised that EPSR is not just a simple classical Monte Carlo code performing a simulation of the specified system. Through the comparison of measured and calculated structure factor data, the interatomic potentials used in the simulation are modified in order to drive the simulation towards agreement with the experiment. In this manner, the simulated representation of the system is consistent with that measured experimentally, meaning that structural properties can be reliably extracted and discussed. EPSR thus performs atomistic simulations that are at least as good as, but most often better, than standard ‘static’ potential-based techniques such as molecular dynamics, since the discrepancies with measured experimental data are utilised in the process.

As well as the need to provide succession planning in light of the retirement of Prof. Soper, there is also the need to develop a code that is designed from the outset to handle the problems of increasing complexity that are now frequently encountered as part of the NIMROD and SANDALS user programmes. These may include, but are not limited to:

1. Porous systems involving confined liquids
2. Self-assembly of molecular subunits into larger structures, e.g. micelles, segregation of liquids
3. Large molecule simulation (e.g. proteins)
4. Ionic systems where the degree of dissociation is unknown, or is affected by external factors

Dissolve is intended to handle these, and future, cases in order to enhance and extend the scientific output of the Disordered Materials group.
