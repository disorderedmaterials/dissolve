# Technical Specification

## Scope
This document provides a high-level overview of the Dissolve code, its primary purpose, and the technical architecture utilised.

## Overview
Dissolve is a tool to analyse experimental scattering data through classical atomistic modelling of the underlying systems. It's purpose is to replicate and extend the functionality of an existing code, Empirical Potential Structure Refinement. It's primary user base is that of the Disordered Materials group at the ISIS Pulsed Neutron and Muon Source, STFC Rutherford Appleton Laboratory, Harwell Oxford, UK.

Within the Disordered Materials group there is a significant driver and need for the simulation of experimental scattering data, in order to obtain the best from the user programme experiments. While isotopic substitution of target systems provide multiple datasets by which to investigate the structure of disordered systems, for all but the simplest systems it is not possible to perform enough isotopic substitutions in order to uniquely extract all the partial structure factors / radial distribution functions of interest. Molecular simulation of the obtained data is, therefore, key to obtaining the full set of partials in a consistent and realistic manner.

To date, the solution to this problem has been provided in the form of the Empirical Potential Structure Refinement (EPSR) code developed over the past 20 years by Prof. A. K. Soper. EPSR has driven (and continues to drive) the scientific output of the Disordered Materials group, through allowing users to probe structural features of their targeted systems that would otherwise not be possible using isotopic substitution alone.  Crucially, it must be recognised that EPSR is not just a simple classical Monte Carlo code performing a simulation of the specified system. Through the comparison of measured and calculated structure factor data, the interatomic potentials used in the simulation are modified in order to drive the simulation towards agreement with the experiment. In this manner, the simulated representation of the system is consistent with that measured experimentally, meaning that structural properties can be reliably extracted and discussed. EPSR thus performs atomistic simulations that are at least as good as, but most often better, than standard ‘static’ potential-based techniques such as molecular dynamics, since the discrepancies with measured experimental data are utilised in the process.
As well as the need to provide succession planning in light of the forthcoming retirement of Prof. Soper, there is also the need to develop a code that is designed from the outset to handle the problems of increasing complexity that are now frequently encountered as part of the NIMROD and SANDALS user programmes. These may include, but are not limited to:

1. Porous systems involving confined liquids
2. Self-assembly of molecular subunits into larger structures, e.g. micelles, segregation of liquids
3. Large molecule simulation (e.g. proteins)
4. Ionic systems where the degree of dissociation is unknown, or is affected by external factors

## Baseline Requirements

The baseline set of requirements for Dissolve is:

1. Provide the ability to simulate large and complex systems studied by total scattering in line with the demands of the NIMROD/SANDALS user programme. This should encompass the ability to perform refinement of atomic systems of millions of atoms, in reasonable timescales, as well as those systems considered ‘complex’ and which are not treatable by single state points / configurations (e.g. porous systems where the pores are well-defined geometrically, but span some range of pore radii).
2. Provide an intuitive and easy-to-use set of tools for the setup, running, and analysis of the code for a given problem. The potential user base cannot be assumed to be experts in, or even familiar with molecular simulation and command-line tools, and so a next-generation code should be usable by all skill tiers of the user community without compromising the demands of any particular user level. . Where  possible, the code should aim to limit additional learning for those already familiar with the existing code base (EPSR).
3. Mimic, to the best degree possible within the framework of the new code, the functionality of the existing EPSR software, so as to cover the needs and expectations of the existing user community.
4. Provide alternative routes, beyond those found within EPSR, for the fitting and interrogation of experimental data. This may encompass alternative fitting procedures (e.g. Simplex minimisation of the interaction parameter space, either through standard functional forms or Spline-based potential representations), the ability to interactively adjust interactions between molecule types / functional groups (e.g. sandbox-style approach where the user strengthen / weaken interactions in the system through chemical intuition to see the effects on the structure), and alternative mechanisms of simulation (e.g. molecular dynamics or Grand-Canonical Monte Carlo).

## Technical Architecture

Dissolve stores the definitions for, and operates on, one or more _configurations_. A _configuration_ is a representation of an experimentally-measured system, comprising atoms at specific positions, the chemical bonds between them, and the molecular units that they represent. Since the nature, size, and content of the experimentally-measured systems is by nature extremely varied, Dissolve does not assume any default analysis workflow or path. Instead, specific operations / algorithms / calculations are implemented as separate _modules_, and which are assembled into a workflow on a case-by-case basis, specified by the user, in order to best analyse the current system under study.

The main Dissolve class, therefore, maintains a list of current configurations, a list of available modules, and one or more sequences of modules to run during its main loop.



## Main Workflow

- 
