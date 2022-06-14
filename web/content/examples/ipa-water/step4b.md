---
title: Step 4b - Add Site Orientation
type: docs
weight: 1
---

To define a site with a specific reference orientation it is your responsibility to provide atom indices for the origin and the representative x and y directions - Dissolve will do the rest. Further information about defining axes on centre-of-mass sites, is shown in [Step9c](https://projectdissolve.com/examples/water/step9c/) of the "Liquid Water" tutorial. We will continue by:

{{< action type="tabs" >}}Go to the {{< gui-tab text="Water" type="Species" >}} species tab and open the **Analysis Sites** section {{< /action >}}
{{< step text="Make sure the COM-w site is selected in the list on the left" />}}
{{< action type="mouse" >}}Select the oxygen atom and click the {{< gui-button text="X Axis" >}} button in the viewer toolbar{{< /action >}}
{{< action type="mouse" >}}Select either one of the hydrogen atoms and click the {{< gui-button text="Y Axis" >}} button in the viewer toolbar{{< /action >}}

You'll note that, as soon as you pressed the {{< gui-button text="Y Axis" >}} button a set of axes appeared in the site viewer, letting you know that your definition is complete and showing you how your system of axes is oriented.

We need to do something similar for the IPA molecule:

{{< action type="tabs" >}}Go to the {{< gui-tab text="IPA" type="Species" >}} species tab and open the **Analysis Sites** section {{< /action >}}
{{< step text="Make sure the COM-IPA site is selected in the list on the left" />}}
{{< action type="mouse" >}}Select the oxygen atom of the alcohol moiety and click the {{< gui-button text="X Axis" >}} button in the viewer toolbar{{< /action >}}
{{< action type="mouse" >}}Select one of the methyl groups and click the {{< gui-button text="Y Axis" >}} button in the viewer toolbar{{< /action >}}

It is also necessary to create an analysis site for the oxygen atom of the IPA molecule. The below step will create an analysis site named O:

{{< action type="mouse" >}}Select only the oxygen atom of the IPA molecule and click the {{< gui-button text="Create" >}} button in the viewer toolbar{{< /action >}}

{{< button pos="left" text="Previous Step" path="step4a/">}} {{< button pos="right" text="Next Step" path="step5/">}}
