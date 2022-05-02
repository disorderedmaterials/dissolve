---
title: Step 2 - Set up a Configuration
type: docs
---

Time to create a suitable liquid configuration for our simulation using the predefined "simple random mix" generator:

{{< action type="menu" text="Configuration &#8680; Create &#8680; Simple random mix" />}}
{{< step text="Choose the benzene species and press **Select**" />}}
{{< action type="edit" text="Double-click on the configuration tab's title and change its name to `Liquid`" />}}

In the interests of speed, we'll use the default number of molecules (100) for the simulation, but we will need to set the correct density. We'll use chemical units of g/cm<sup>3</sup> to specify it:

{{< action type="mouse">}} Select the {{< gui-node "Parameters" >}} node in the generator{{< /action >}}
{{< action type="edit">}}Set the value of the \"rho\" variable to `0.876`. Note that this is \"just a number\" - units are given in the {{< gui-node "Add" >}} node when we reference this value{{< /action >}}
{{< action type="mouse">}}Select the  the {{< gui-node "Add" >}} node in the generator{{< /action >}}
{{< action type="edit" text="The **Density** keyword will already reference the parameter \"rho\", but the units will need to be changed to `g/cm3`" />}}
{{< action type="mouse">}}Click {{< gui-button text="Generate" icon="general_repeat" >}} to recreate the configuration with our adjusted parameters{{< /action >}}

The resulting box will have a side length of just over 24.5 &#8491;.

{{< cimage src="../benzenebox.png" caption="Initial, randomised benzene box containing 100 molecules" >}}

Since the molecules were added to the box completely at random, there will most likely be some that overlap quite a lot. Benzene has a ring which makes it possible for molecules to become permanently "interlocked", but we can do our best to remove such contacts by using what Dissolve (and EPSR) call "size factor scaling". Basically, this allows the configuration box and the positions of the molecules to be scaled (but keeping the internal geometry intact), removing bad overlaps and letting molecules move safely apart. The factor by which the box is scaled is reduced gradually until the initial requested system size is regained (i.e. corresponding to a size factor of 1.0).

In anticipation of some bad contacts:

{{< action type="tabs">}}Go to the {{< gui-tab type="configuration" text="Liquid" >}} configuration tab{{< /action >}}
{{< step text="Set the **Requested** value for the _Size Factor Scaling_ to 10.0" />}}


Finally, before we move on, we will have to reduce our pair potential range from the default of 15 &#8491; since this is larger than the half the maximum width of our simulation box.

{{< action type="tabs">}} Go to the {{< gui-tab type="ff" text="Forcefield" >}} tab, **Atom Types / Pair Potentials** section{{< /action >}}
{{< action type="edit" text="Reduce the pair potential **Range** from `15` to `12` &#8491;" />}}


{{< tip text="For a cubic box, you can't have a cutoff value greater than half the box length, as this means that a given atom will 'see' other atoms twice because of the periodic boundary conditions." />}}

* * *
{{< button pos="left" text="Previous Step" path="step1/">}}
{{< button pos="right" text="Next Step" path="step3/">}}
