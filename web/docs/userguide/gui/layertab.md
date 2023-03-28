---
title: Layer Tab
description: Layer tabs
weight: 4
---

A Layer tab exposes a single simulation "layer" - a sequence of modules which do some kind of useful work to achieve the desired result. The entire layer can be removed by clicking the close button associated with the tab.

The frequency at which the layer is executed, as well as a general on/off switch for the layer, is located at the top left of the tab. 

### Modules and Options

{{< cimage src="../layertab_1.png" caption="Module tab showing standard options for a selected module" width=90% >}}

#### Run Control

It is possible to dynamically control the execution of a layer by the use of the run control settings - these provide mechanisms to prevent a layer from running under certain conditions:

- _Energy Stability_ - If selected the layer will not run if the energies of any targeted configuration are not stable (as determined by an {{< module "Energy" >}} module).
- _Size Factors_ - If selected the layer will not run if the size factor of any target configuration has not yet returned to 1.0. 

#### Modules

The constituent modules of the current layer are shown here, along with options to enable / disable individual modules and adjust their run frequency. New modules can be added to the layer through the _Show Available Modules_ button at the bottom of the list.

Selecting a module in the list will show any available options and output on the right-hand side of the tab. Underneath the name and type of the module are a series of buttons allowing access to controls for the module - every module has an "Options" section containing its basic / common controls, most have an "Output" section, and some have an "Advanced" section. The latter typically contains options that do not normally need to be modified in standard workflows, and appears to the far right of the screen.

{{< cimage src="../layertab_2.png" caption="Module tab showing output for a selected module" width=90% >}}
