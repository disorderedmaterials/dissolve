---
title: Step 4 - Make the Ionic Liquid
type: docs
weight: 1
---

Time to create a configuration to represent the ionic liquid.

### Generate the Configuration

{{< action type="menu" text="Configuration &#8680; Create..." />}}
{{< action type="mouse" >}}Choose both species and press {{< gui-button text="Next" icon="arrowright" >}}{{< /action >}}
{{< action type="mouse" >}}Leave the configuration type as _Mixture_ and press {{< gui-button text="Next" icon="arrowright" >}}{{< /action >}}
{{< action type="mouse" >}}Leave the box style as _Fixed Geometry, Undefined Size_ and press {{< gui-button text="Next" icon="arrowright" >}}{{< /action >}}
{{< action type="edit" >}}Set the _Density_ to `0.09985` and the units to `atoms/A3`, and the _Multiplier_ to `100`. This gives us a box with side length of 29.6398 &#8491; which is quite small for an ionic liquid, but in the interests of time we'll leave it as that. Press {{< gui-button text="Finish" icon="true" >}} to complete the wizard.{{< /action >}}
{{< action type="edit">}}Double-click on the configuration tab's title and change its name to {{< gui-tab type="configuration" text="Bulk" >}}{{</action>}}

We need to adjust our pair potential range which is currently set to 15 &#8491; and exceeds the half-cell width:

{{< action type="tabs">}}Go to the {{< gui-tab type="ff" text="Forcefield" >}} tab, {{< gui-button text="Pair Potentials" >}} section{{< /action >}}
{{< action type="edit" >}}In the _Pair Potentials_ controls, change the _Range_ to `12.0`{{< /action >}}

The final thing to do is change the temperature (our experimental data was measured at 323 K) and set a size factor for our box - since we have rings in our cation we need to apply a size factor during the initial equilibration to help prevent occurrences of interlocking rings.

{{< action type="tabs">}}Go to the {{< gui-tab type="configuration" text="Bulk" >}} configuration tab{{< /action >}}
{{< action type="groups">}}Click the {{< node "Temperature">}} node{{< /action >}}
{{< action type="edit" >}}Set the **Temperature** to `323.0`{{< /action >}}
{{< action type="groups">}}Click the {{< gui-button icon="palette" text="Show Avaliable Nodes">}} button{{< /action >}}
{{< action type="mouse">}}Drag the {{<node "SizeFactor">}} node into the Node list{{< /action >}}
{{< action type="groups">}}Select the {{< node "SizeFactor">}} node{{< /action >}}
{{< action type="edit" >}}Set the SizeFactor to `10.0`{{< /action >}}
{{< action type="mouse" >}}Click {{< gui-button text="Generate" icon="repeat" >}} to update the configuration.{{< /action >}}

{{< cimage src="../initial_box.png" caption="Initial box of 1-ethyl-3-methylimidazolium acetate" >}}


### Equilibrate

Let's now add a suitable evolution layer for the system and set things running:

{{< action type="menu" text="Layer &#8680; Create &#8680; Evolution &#8680; Standard Molecular (MC/MD)" />}}

Now we can equilibrate our system:

{{< action type="key" >}}Ctrl-F{{< /action >}}
{{< step >}}Set the number of iterations  to `1000` and hit {{< gui-button text="OK" >}}{{< /step >}}

You might want to track the change in energy of the system in the {{< gui-button text="Output" icon="output" >}} tab of the {{< module "Energy" >}} module, in the {{<gui-tab type="layer" text="Evolve (Standard)" >}} tab.

After one thousand iterations the size factor of the configuration should have returned to 1.0 (check this on the configuration tab!) and the intermolecular energy of the system should have reached a stable negative value of about &ndash;100 kJ mol<sup>-1</sup> (for 100 ion pairs). The charged nature of the species in the present system means that reaching equilibrium can take a lot longer than for a system containing only small, neutral molecules - bear this in mind when running such systems. Also note that the _total_ energy of the configuration remains large and positive due to the intramolecular energy contributions.

{{< cimage src="../equilibrated_box.png" caption="Equilibrated box of 1-ethyl-3-methylimidazolium acetate" >}}

* * *
{{< button pos="left" text="Previous Step" path="step3/">}}
{{< button pos="right" text="Next Step" path="step5/">}}
