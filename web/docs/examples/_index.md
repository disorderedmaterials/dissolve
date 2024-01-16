---
title: Examples
weight: 2
type: docs
---

Here you can find various usage examples, tutorials, and step-by-step walkthroughs for Dissolve.

## Read Me First!

It is worth understanding the two main files used by Dissolve and their respective purposes.

The **input file** describes the contents and behaviour of a simulation - i.e. the forcefield and species, how configurations should be built, and what should be done to (or calculated from) those configurations. This is the file created and saved by the Dissolve GUI as you set up your simulation.

When you finally _run_ your simulation - i.e. ask Dissolve to do what you've described in the input file - it will generate a **restart file** with a `.restart` suffix. This file contains all the output generated from running your simulation - as a minimum this contains the current coordinates of any configurations, but also output from modules (radial distribution functions, structure factors, analyses quantities etc.), pair potential modifications, and certsin "system" values (timing information for modules, and the current iteration number).

The restart file should be considered quite precious as it is used by Dissolve to begin a simulation "from where it left off" if, for instance, your computer gets turned off, you need to go back to a previous "good" point in time, or (Heaven forbid) Dissolve crashes. Feel free to take copies of the restart file whenever you like. Better safe than sorry.

## Associated Data Files

Relevant data as well as fully-constructed input files (for reference) can be downloaded as a {{< exampledatalink linkText="zip" urlSuffix="zip" >}} or {{< exampledatalink linkText="tar.gz" urlSuffix="tar.gz" >}} from the [releases page](https://github.com/disorderedmaterials/dissolve/releases). Alternatively, you can download individual files from the main [GitHub repo](https://github.com/disorderedmaterials/dissolve/tree/develop/examples).

## Starting Out

The following examples represent fully-explained, detailed step-by-step instructions suitable for absolute beginners to Dissolve and/or total scattering analysis.

{{< cardpane >}}
  {{< card header="**Argon**" title="Beginner's Guide"
          footer="[Get Started!](argon/)">}}
  {{< cimage src="argon/icon.png" url="argon/" width="200px" height="200px" >}}
  {{< /card >}}
  {{< card header="**Bulk Silica**" title="Creating and refining an atomic glass"
          footer="[Get Started!](silica/)">}}
  {{< cimage src="silica/icon.png" url="silica/" width="200px" height="200px" >}}
  {{< /card >}}
  {{< card header="**Liquid Water**" title="Analysing a simple liquid"
          footer="[Get Started!](water/)">}}
  {{< cimage src="water/icon.png" url="water/" width="200px" height="200px" >}}
  {{< /card >}}
  {{< card header="**Benzene**" title="Investigations into the archetypal aromatic"
          footer="[Get Started!](benzene/)">}}
  {{< cimage src="benzene/icon.png" url="benzene/" width="200px" height="200px" >}}
  {{< /card >}}
{{< /cardpane >}}

## Intermediate Examples

Examples with less detailed instructions, suitable for users who have completed a few of the starting out examples. Some include follow-on exercises for the reader to challenge your use / knowledge of Dissolve.

{{< cardpane >}}
  {{< card header="**An Ionic Liquid**" title="The ionic liquid 1-ethyl-3-methylimidazolium acetate"
          footer="[Get Started!](emim-oac/)">}}
  {{< cimage src="emim-oac/icon.png" url="emim-oac/" width="200px" height="200px" >}}
  {{< /card >}}
  {{< card header="**Framework Systems**" title="Creating and simulating a periodic metal organic framework"
          footer="[Get Started!](cu-btc/)">}}
  {{< cimage src="cu-btc/icon.png" url="cu-btc/" width="200px" height="200px" >}}
  {{< /card >}}
{{< /cardpane >}}

## Usage Examples

Shorter tutorials on how to use a specific aspect of Dissolve, or make the most of particular capability.

{{< cardpane >}}
  {{< card header="**Importing from LigParGen**" title="Using the LigParGen service to obtain species forcefields"
          footer="[Get Started!](ligpargen/)">}}
  {{< cimage src="ligpargen/icon.png" url="ligpargen/" width="200px" height="200px" >}}
  {{< /card >}}
  {{< card header="**Dissolve as a Post-Processor**" title="Using Dissolve as a post-processing tool for external data"
          footer="[Get Started!](post-processing/)">}}
  {{< cimage src="post-processing/icon.png" url="post-processing/" width="200px" height="200px" >}}
  {{< /card >}}
{{< /cardpane >}}
