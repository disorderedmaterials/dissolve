---
title: Step 2 - Set up a Configuration
type: docs
---

Time to create a suitable liquid configuration for our simulation using the predefined "simple random mix" generator:

{{< action type="menu" text="Configuration &#8680; Create... &#8680; Simple random mix" >}}
{{< step text="Choose the benzene species and press **Select**">}}
{{< action type="edit" text="Double-click on the tab title and change its name to `Liquid`" >}}

In the interests of speed, we'll use the default number of molecules (100) for the simulation, but we will need to set the correct density. We'll use chemical units of g/cm<sup>3</sup> to specify it:

{{< action type="settings" text="Open the settings for the `Parameters` node" >}}
{{< step text="Check that the value of the \"rho\" variable is 0.876. Note that this is \"just a number\" - units are given in the `AddSpecies` node when we reference this value">}}
{{< action type="settings" text="Open the settings for the `AddSpecies` node" >}}
{{< step text="Make sure that the units for the **Density** are set to `g/cm3` and that it references our parameter `rho`">}}
{{< action type="mouse" text="Click **Regenerate** to recreate the configuration with our adjusted parameters" >}}

The resulting box will have a side length of just over 24.5 &#8491;.

{{< cimage src="../benzenebox.png" caption="Initial, randomised benzene box containing 100 molecules" >}}

Since the molecules were added to the box completely at random, there will most likely be some that overlap quite a lot. Since our molecule has a ring, this can lead to situations where molecules become interlocked, and which will persist throughout the simulation unless we deal with them. The easiest thing to do is to use what Dissolve (and EPSR) call "size factor scaling". Basically, this allows the configuration box and the positions of the molecules to be scaled temporarily, removing bad overlaps and letting molecules move safely apart. The factor by which the box is scaled is reduced gradually until a value of 1.0 (i.e. the initial system size) is regained.

In anticipation of some bad contacts:

{{< action type="tabs" text="**Liquid** configuration tab" >}}
{{< step text="Set the **Requested** value for the _Size Factor Scaling_ to 10.0" >}}


Finally, before we move on, we will have to reduce our pair potential range from the default of 15 &#8491; since this is larger than the half the maximum width of our simulation box.

{{< action type="tabs" text="**Forcefield** tab, **Pair Potentials** section" >}}
{{< step text="Reduce the pair potential **Range** from 15 to 12 &#8491;" >}}


{{< tip text="For a cubic box, you can't have a cutoff value greater than half the box length, as this means that a given atom will 'see' other atoms twice because of the periodic boundary conditions." >}}

{{< button pos="left" text="Previous Step" path="step1/">}}
{{< button pos="right" text="Next Step" path="step3/">}}
