---
title: 6. The GUI
description: An overview of Dissolve's GUI
weight: 6
---

### Overview

The GUI of Dissolve looks like this (displaying the example water input file):

{{< cimage src="mainwindow.png" caption="Dissolve GUI with water simulation loaded" width=90% >}}

A series of tabs represent all the different objects and component parts of your simulation. The order of this data is and important and read left-to-right represents how the simulation built-up within Dissolve.

{{< tip text="The availability of main menu items depends on the content of the currently-selected tab." />}}

Tabs that represent objects in the simulation (e.g. species, configurations etc.) can be renamed by double-clicking on the tab itself. Closing a tab will delete that object from the simulation (you will be asked by Dissolve if this is really what you want to do). Note that the Messages and Forcefield tabs can't be renamed or closed.

### Messages

Any output from Dissolve is sent to the Messages tab - when Dissolve is running it will show the output from the current iteration, updated as the simulation layers are processed. Keep an eye on the content here to make sure the correct things are happening. Especially, check here for clues as to why simulations aren't running, for instance.

### Status Bar

The status bar at the bottom of Dissolve's main window will tell you the current status ("Idle", "Running" etc.) and, equally importantly, will notify you when errors are detected. If errors are reported, go to the "Messages" tab to get more information on what went wrong.

## Running a Simulation

There are pros and cons of running the simulation through the GUI. On the one hand, you can investigate / plot / interrogate the simulation as it is running. On the other, the GUI invokes some overhead from displaying all the information, and so is not as performant as the CLI version. However, it is multithreaded just like the command-line version, so can be used for production simulations as well.

The "Simulation" menu allows simulations to be stopped and started, or the following keyboard shortcuts can be used instead:

|Shortcut|Action|
|:-----:|------|
|`Ctrl-R`|Run the simulation until interrupted.|
|`Esc`|Stop a running simulation. This may not happen immediately since the current iteration will always run to completion.|
|`Ctrl-1`|Perform a single iteration.|
|`Ctrl-5`|Perform five iterations.|
|`Ctrl-F`|Run for a specific number of iterations - a dialog will pop up asking for the number to run.|


