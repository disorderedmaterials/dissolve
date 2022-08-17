---
title: Examples
weight: 2
type: docs
---

Here you can find various usage examples, tutorials, and step-by-step walkthroughs for the current release of Dissolve (the latest version being {{< releaseversion >}}). Examples for earlier versions can be [found here](previous/0.8/).

### Instructions

Throughout the examples specific actions to perform in the GUI are given in banners with blue markers as follows:

- Interact with a particular control in the GUI

{{< action type="mouse" text="Click on the `NeutronSQ` module to select it" />}}

- Select a menu item

{{< action type="menu" text="File &#8680; New" />}}

- Move to a specific tab

{{< action type="tabs" text="Go to the **Refine (EPSR)** tab" />}}

- Open a specific settings group

{{< action type="groups" text=" Open the **Control** settings group" />}}

- Edit / change a particular value

{{< action type="edit" text="Set the **Temperature** to 323 K" />}}

- Use a specific hotkey

{{< action type="key" text="Ctrl-F" />}}

Additional details for any particular action are provided in sub-banners like so:

{{< action type="menu" text="File &#8680; Save As..." />}}
{{< step text="Save the input file under the name `test.txt`" />}}

Tips and warnings are also highlighted where relevant:

{{< tip text="Tip - Provides a helpful bit of extra advice" />}}

{{< warn text="Warning - Brings something potentially important to your attention" />}}

### Associated Data Files

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
