---
title: 7. Modules
description: Descriptions of all modules and their functionality
weight: 7
---

Modules perform most of the interesting and important functionality present within Dissolve, covering methods to move atoms around, calculation of structure factors and radial distribution functions, and the analysis of configurations amongst other things. Multiple modules, run in sequence, go together to form a full simulation that does something useful with your data. Modules exist within "layers" (often referred to as "processing layers") defined in the main Dissolve simulation.

As well as their own specific configuration keywords (as detailed on the respective manual pages) every module recognises a set of [basic keywords]({{< ref "moduleblock#basic-keywords" >}}) controlling their basic operation.
