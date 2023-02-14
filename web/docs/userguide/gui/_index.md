---
title: 6. The GUI
description: An overview of Dissolve's GUI
weight: 6
---

## Overview

The GUI of Dissolve looks like this (displaying the example water input file):

{{< cimage src="mainwindow.png" caption="Dissolve GUI with water simulation loaded" width=90% >}}

### Run Controls

{{< cimage src="../runcontrols.png" caption="Run controls on the left-hand side of the main window" >}}

Controls to start, stop, and step the current simulation within the GUI can be found on the left-hand side of the main window, along with the main simulation loop counter.

### Content Tabs

{{< cimage src="../tabs.png" caption="Content tabs displaying aspects of the current simulation" >}}

The important content of the simulation - species, configurations, layers etc. - is shown in separate tabs in the central area. The majority of set up, editing, and monitoring is performed via these tabs. Note that some main menu items are enabled / disabled based on the content of the currently-selected tab.

| Tab  | Content   |
|:----------------------|:-------|
| ![](../forcefieldtab.png) | Forcefield tab - contains common forcefield parameters such as master terms and pair potentials |
| ![](../speciestab.png) | Species tabs - display / edit individual species |
| ![](../configurationtab.png) | Configuration tabs - display / edit individual configurations |
| ![](../layertab.png) | Layer tabs - display / edit sequences of modules |
| ![](../moduletab.png) | Module tabs - show keyword options and output from individual modules (the icon reflects the module type) |
| ![](../workspacetab.png) | Workspace tab - display a custom selection of modules and their options / output |

To rename a tab, double-click on the tab itself. This also changes the name of any object that the tab represents (e.g. the displayed Species). Closing a tab will delete that object from the simulation (you will be asked by Dissolve if this is really what you want to do).

The order of tabs is fixed, and represent how the simulation is built-up within Dissolve. Forcefield terms are used to describe species which then make up configurations, and layers do useful things to our configurations.

### Messages Box

{{< cimage src="../messages.png" >}}

All of Dissolve's output (information, warnings, errors, and, of course, good things) is printed to the messages box. Keep an eye on the content here to make sure the correct things are happening. Especially, check here for clues as to why simulations aren't running, for instance.



## Running a Simulation

There are pros and cons of running the simulation through the GUI. On the one hand, you can investigate / plot / interrogate the simulation as it is running. On the other, the GUI invokes some overhead from displaying all the information, and so is not as performant as the CLI version. However, as of version 0.8 the GUI is multithreaded, so can be used for production simulations of small to medium systems.

The "Simulation" menu allows simulations to be stopped and started, or the following keyboard shortcuts can be used instead:

|Shortcut|Action|
|:-----:|------|
|`Ctrl-R`|Run the simulation until interrupted.|
|`Esc`|Stop a running simulation. This may not happen immediately since the current iteration will always run to completion.|
|`Ctrl-1`|Perform a single iteration.|
|`Ctrl-5`|Perform five iterations.|
|`Ctrl-F`|Run for a specific number of iterations - a dialog will pop up asking for the number to run.|

The status bar at the bottom of Dissolve's main window will tell you the current status ("Idle", "Running" etc.) and, equally importantly, will show you when errors are detected. If errors are reported, go to the "Messages" tab to get more information on what went wrong.
