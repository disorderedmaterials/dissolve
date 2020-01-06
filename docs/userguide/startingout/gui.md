---
title: Dissolve's GUI
parent: Starting Out
grand_parent: User Guide
nav_order: 2
---
# Dissolve's GUI

2 minute read
{: .label .label-blue }

## Overview
Dissolve's user interface comprises three main areas:

### Run Controls

![Run controls on the left-hand side of the main window](runcontrols.png){: .img-centre}

Controls to start, stop, and step the current simulation within the GUI can be found on the left-hand side of the main window, along with the main simulation loop counter.

### Content Tabs

![Content tabs displaying aspects of the current simulation](tabs.png){: .img-centre}

The important content of the simulation - species, configurations, layers etc. - is shown in separate tabs in the central area. The majority of set up, editing, and monitoring is performed via these tabs. Note that some main menu items are enabled / disabled based on the content of the currently-selected tab.

| ![Forcefield tab](forcefieldtab.png) | Forcefield tab - contains common forcefield parameters such as master terms and pair potentials |
| ![Species tab](speciestab.png) | Species tabs - display / edit individual species |
| ![Configuration tab](configurationtab.png) | Configuration tabs - display / edit individual configurations |
| ![Layer tab](layertab.png) | Layer tabs - display / edit sequences of modules |
| ![Module tab](moduletab.png) | Module tabs - show keyword options and output from indivdiual modules (the icon reflects the module type) |
| ![Workspace tab](workspacetab.png) | Workspace tab - display a custom selection of modules and their options / output |

To rename a tab, double-click on the tab itself. This also changes the name of any object that the tab represents(e.g. the displayed Species). Closing a tab will delete that object from the simulation (you will be asked by Dissolve if this is really what you want to do).

The order of tabs is fixed, and represent how the simulation is built-up within Dissolve. Forcefield terms are used to describe species which then make up configurations, and layers do useful things to our configurations.

### Messages Box

![Messages box](messages.png){: .img-centre}

All of Dissolve's output (information, warnings, errors, and sometimes good things) is printed to the messages box. Keep an eye on the content here to make sure the correct things are happening. Especially, check here for clues as to why simulations aren't running, for instance.
