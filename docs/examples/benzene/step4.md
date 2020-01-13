---
title: Step 4 - Set up Analysis
parent: Comparing Benzene Structure
grand_parent: Examples
---
# Comparing Benzene Structure

## 3. Set up Analysis

Let's add an empty processing layer to to the simulation and add on a couple of modules:

> Layer &#8680; Create... &#8680; Analysis... &#8680; Empty
{: .action .action_menu}
> Show the module palette for the current layer by clicking the _Show Palette_{: .text-green-100} button at the top left of the tab
{: .action .action_mouse}

### 

### Radial Distribution Function

Firstly, we'll add a [`CalculateRDF`](/userguide/modules/calculaterdf) module to provide us with the centre-of-geometry radial distribution function.

> Drag a [CalculateRDF](/userguide/modules/calculatedrdf) module over to the **Current Modules** list
{: .action .action_mouse}
> Select the [CalculateRDF](/userguide/modules/calculatedrdf) module to show its options
{: .step}
> Open the **Sites** settings group
{: .action .action_groups}
> Set both **SiteA** and **SiteB** to `COG`
{: .step}
> Enable the **ExcludeSameMolecule** to prevent the unwanted self-correlation spike at 0 in the resulting RDF
{: .step}

[Previous Step](step9b.md){: .btn }   [Next Step](step10.md){: .btn .right}
