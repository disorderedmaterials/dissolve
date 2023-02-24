---
title: Step 2 - Set up a Configuration
type: docs
weight: 4
---


Time to create a suitable bulk water configuration for our simulation.

{{< action type="menu" text="Configuration &#8680; Create..." />}}
{{< action type="mouse" >}}Choose the water species and press {{< gui-button text="Next" icon="general_arrowright" >}}{{< /action >}}
{{< action type="mouse" >}}Leave the configuration type as _Mixture_ and press {{< gui-button text="Next" icon="general_arrowright" >}}{{< /action >}}
{{< action type="mouse" >}}Leave the box style as _Fixed Geometry, Undefined Size_ and press {{< gui-button text="Next" icon="general_arrowright" >}}{{< /action >}}
{{< action type="edit" >}}The _Density_ we need is `0.1` `atoms/A3` which just happens to be the default, and the standard _Multiplier_ of `1000` is good for us, so press {{< gui-button text="Finish" icon="general_true" >}} to complete the wizard.{{< /action >}}
{{< action type="edit" text="Double-click on the configuration tab's title and change its name to `Bulk`" />}}


One thousand water molecules should have a cubic box size of just over 31 &#8491; side length. You will see in the {{< gui-node "Add" >}} node that its **Species** option is not set to anything, which seems counter-intuitive. Specifying a species here instructs the {{< gui-node "Add" >}} node that $N$ identical copies of the species should be added to the box. In most cases it is preferable to introduce some variety to the geometry of the molecules, and so the {{< gui-node "Add" >}} node can instead reference a {{< gui-node "CoordinateSets" >}} node as the source, and which contains sets of coordinates representing randomised, evolved, or read in (from a file) coordinates for the species.

Before we proceed we'll tweak the temperature of the configuration.

{{< action type="edit" text="Set the **Temperature** to 298 K" />}}

{{< cimage src="../waterbox.png" caption="Initial, randomised water box containing 1000 molecules" >}}

* * *
{{< button pos="left" text="Previous Step" path="step1b/">}}
{{< button pos="right" text="Next Step" path="step3/">}}
