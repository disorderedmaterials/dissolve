---
title: Step 3 - Import into Dissolve
type: docs
weight: 1
---

We're now ready to get this data into Dissolve, so fire it up if you haven't already and create a new, empty simulation.

Then:

{{< action type="menu" text="Species &#8680; Import... &#8680; From LigParGen..." />}}
{{< step text="The wizard first asks you for the OpenMM (`XML`) and `XYZ` data that LigParGen provided, so..." />}}
{{< action type="edit" >}}Set the _OpenMM File_ to the xml file from LigParGen{{< /action >}}
{{< step text="If the corresponding xyz file was in the same location as the xml it will have been automatically selected by Dissolve. If not, select it now." />}}
{{< action type="mouse" >}}Press {{< gui-button text="Next" icon="arrowright" >}} to continue{{< /action >}}

The wizard will then show you the 3D structure as read in from the xyz file, just for you to confirm.

{{< action type="mouse" >}}Assuming everything looks good you can press {{< gui-button text="Next" icon="arrowright" >}} to continue.{{< /action >}}

You'll then be presented with a breakdown of all the forcefield terms that have been read in from the xml file. While it isn't necessary to review all of these, they are given to show you what has been found and to highlight any obvious failings of the process (e.g. zero bond terms etc.).  Take a look through the data if you like.

{{< action type="mouse" >}}Press {{< gui-button text="Next" icon="arrowright" >}} to continue.{{< /action >}}

The wizard then focuses specifically on how to introduce the terms defined in the  LigParGen data. It's often the case that, except for very small molecules, there will exist atom types or intramolecular interactions (e.g. bond or angle terms) that use identical parameters. On those occasions it's good to fold identical atom types into one, and reduce the intramolecular parameter sets into terms into master terms - we like master terms in Dissolve! Those options are enabled here by default.

{{< action type="mouse" >}}Press {{< gui-button text="Next" icon="arrowright" >}} to continue.{{< /action >}}

The final page of the wizard is for handling creation of a forcefield object that can be applied to other species, but at present (as well as here) we don't use it.

{{< action type="mouse" >}}click {{< gui-button text="Finish" icon="true" >}} to exit the wizard.{{< /action >}}

* * *
{{< button pos="left" text="Previous Step" path="step2/">}}
{{< button pos="right" text="Next Step" path="step4/">}}
