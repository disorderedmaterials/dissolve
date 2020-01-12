---
title: Step 4 - Set up Analysis
parent: Comparing Benzene Structure
grand_parent: Examples
---
# Comparing Benzene Structure

## 3. Set up Analysis

Let's add a processing layer to calculate the average molecule, the centre-of-mass RDF and the centre-of-mass spatial density from our equilibrated system.

> Layer &#8680; Create... &#8680; Analysis... &#8680; Average Molecule & SDF
{: .action .action_menu}

### Average Molecule

For the average molecule calculation made by the [`CalculateAvgMol`](/userguide/modules/calculateavgmol) module all we need to do is set the target site (which must contain defined axes):

> Select the [`CalculateAvgMol`](/userguide/modules/calculateavgmol) module to display its options
{: .action .action_mouse}
> Open the **Target** settings group
{: .action .action_groups}
> Set the target **Site** for the average molecule calculation to be `COG` (it will be the only one available, since it is the only one which has a set of axes defined)
{: .step}

### Spatial Density Function

We only have one site - the centre-of-geometry (`COG`) - so out spatial density function will represent the distribution of molecule centres about a central benzene molecule:

> Select the [`CalculateSDF`](/userguide/modules/calculatesdf) module to display its options
{: .action .action_mouse}
> Open the **Sites** settings group
{: .action .action_groups}
> Set the central **SiteA** to `COG` (again, the only one available)
{: .step}
> Set the surrounding **SiteB** to `COG` also
{: .step}

### Radial Distribution Function

To the end of our analysis layer we'll add a [`CalculateRDF`](/userguide/modules/calculaterdf) module to provide us with the centre-of-geometry radial distribution function.

> Show the module palette for the current layer by clicking the _Show Palette_{: .text-green-100} button at the top left of the tab
{: .action .action_mouse}
> Drag a [CalculateRDF](/userguide/modules/calculatedrdf) module over to the **Current Modules** list
{: .step}
> Open the **Sites** settings group
{: .action .action_groups}
> Set both **SiteA** and **SiteB** to `COG`
{: .step}
> Enable the **ExcludeSameMolecule** to prevent the unwanted self-correlation spike at 0 in the resulting RDF
{: .step}

[Previous Step](step9b.md){: .btn }   [Next Step](step10.md){: .btn .right}
