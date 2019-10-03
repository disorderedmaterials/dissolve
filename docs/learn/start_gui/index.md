---
title: Dissolve's GUI
parent: Learn Dissolve
nav_order: 2
---
# Dissolve's GUI

5 minute read
{: .label .label-blue }

## Overview
Dissolve's user interface comprises three main areas:

### Run Controls

![Run controls on the left-hand side of the main window](runcontrols.png){: .img-center}

Controls to start, stop, and step the current simulation within the GUI can be found on the left-hand side of the main window, along with the main simulation loop counter.

### Content Tabs

![Content tabs displaying aspects of the current simulation](tabs.png){: .img-center}

The important content of the simulation - species, configurations, layers etc. - is shown in separate tabs in the central area. The majority of set up, editing, and monitoring is performed via these tabs. Note that some main menu items are enabled / disabled based on the content of the currently-selected tab.

| ![Forcefield tab](forcefieldtab.png) | Forcefield tab - contains common forcefield parameters such as master terms and pair potentials |
| ![Species tab](speciestab.png) | Species tabs - display / edit individual species |
| ![Configuration tab](configurationtab.png) | Configuration tabs - display / edit individual configurations |
| ![Layer tab](layertab.png) | Layer tabs - display / edit sequences of modules |

### Messages Box

![Messages box](messages.png){: .img-center}

All of Dissolve's output (information, warnings, errors, and sometimes good things) is printed to the messages box. Keep an eye on the content here to make sure the correct things are happening. Especially, check here for clues as to why simulations aren't running, for instance.
