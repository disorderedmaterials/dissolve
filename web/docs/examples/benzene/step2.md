---
title: Step 2 - Set up a Configuration
type: docs
---

Time to create a suitable liquid configuration for our simulation using the predefined "simple random mix" generator:

{{< action type="menu" text="Configuration &#8680; Create..." />}}
{{< action type="mouse" >}}Choose the benzene species and press {{< gui-button text="Next" icon="arrowright" >}}{{< /action >}}
{{< action type="mouse" >}}Leave the configuration type as _Mixture_ and press {{< gui-button text="Next" icon="arrowright" >}}{{< /action >}}
{{< action type="mouse" >}}Leave the box style as _Fixed Geometry, Undefined Size_ and press {{< gui-button text="Next" icon="arrowright" >}}{{< /action >}}
{{< action type="edit" >}}Set the _Density_ to `0.876` and the units to `g/cm3`, and the _Multiplier_ to `100`. You'll see that this gives us a box with side length of just over 24.5 &#8491; which is fairly small, but in the interests of speed we'll go with it for now so hit {{< gui-button text="Finish" icon="true" >}}.{{< /action >}}
{{< action type="edit">}}Double-click on the {{< gui-tab type="configuration" text="NewConfiguration" >}} tab's title and change its name to {{< gui-tab type="configuration" text="Liquid">}}{{</action>}}

{{< cimage src="../benzenebox.png" caption="Initial, randomised benzene box containing 100 molecules" >}}

Since the molecules were added to the box completely at random, there will most likely be some that overlap quite a lot. Benzene has a ring which makes it possible for molecules to become permanently "interlocked", but we can do our best to remove such contacts by using what Dissolve (and EPSR) call "size factor scaling". Basically, this allows the configuration box and the positions of the molecules to be scaled (but keeping the internal geometry intact), removing bad overlaps and letting molecules move safely apart. The factor by which the box is scaled is reduced gradually until the initial requested system size is regained (i.e. corresponding to a size factor of 1.0).

In anticipation of some bad contacts:

{{< action type="tabs">}}Go to the {{< gui-tab type="configuration" text="Liquid" >}} configuration tab{{< /action >}}
{{< action type="groups">}}Click the {{< gui-button icon="palette" text="Show Available Nodes" >}} on the bottom-left of the window{{< /action >}}
{{< action type="mouse">}}Drag the {{< node "SizeFactor" >}} node into the **Procedure** section to add it{{< /action >}}
{{< action type="edit" text="Set the value for the _Size Factor_ to 10.0" />}}


Finally, before we move on, we will have to reduce our pair potential range from the default of 15 &#8491; since this is larger than the half the maximum width of our simulation box.

{{< action type="tabs">}} Go to the {{< gui-tab type="ff" text="Forcefield" >}} tab, {{<gui-button text="Pair Potentials">}} section{{< /action >}}
{{< action type="edit" text="In **Control**, reduce the pair potential **Range** from `15` to `12` &#8491;" />}}


{{< tip text="For a cubic box, you can't have a cutoff value greater than half the box length, as this means that a given atom will 'see' other atoms twice because of the periodic boundary conditions." />}}

* * *
{{< button pos="left" text="Previous Step" path="step1/">}}
{{< button pos="right" text="Next Step" path="step3/">}}
