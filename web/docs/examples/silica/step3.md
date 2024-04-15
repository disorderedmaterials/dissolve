---
title: Step 3 - Set up a Configuration
type: docs
---


To create our silica configuration we'll use the "relative random mix" generator as a basis.

{{< action type="menu" text="Configuration &#8680; Create..." />}}
{{< action type="mouse" >}}
Choose both species and press {{< gui-button text="Next" icon="arrowright" >}}{{< /action >}}
{{< action type="mouse" >}}
Leave the configuration type as _Mixture_ and press {{< gui-button text="Next" icon="arrowright" >}}{{< /action >}}
{{< action type="mouse" >}}
Leave the box style as _Fixed Geometry, Undefined Size_ but set the angle **&gamma;** to `120` degrees (this will give us a monoclinic box) and press {{< gui-button text="Next" icon="arrowright" >}}{{< /action >}}

Now we need to set up a relative mix of our two species:

{{< action type="edit" >}}In the table change the _Population / Ratio_ of oxygen from `1` to `2`{{< /action >}}
{{< action type="edit" >}}Set the _Density_ of the box to `0.066` - note that this is in units of atoms &#8491;<sup>-3</sup>{{< /action >}}
{{< action type="edit" >}}Set the _Multiplier_ to `1500`{{< /action >}}
{{< action type="mouse" >}}Press {{< gui-button text="Finish" icon="true" >}} to complete the wizard.{{< /action >}}
{{< action type="edit" text="Double-click on the configuration tab's title and change its name to `Bulk`" />}}


Note that We use units of atoms &#8491;<sup>-3</sup> here as this allows us to add both atomic components using the same density value. If we had chosen units of g cm<sup>-3</sup> we would have to know the individual densities of silicon and oxygen in amorphous silica. Anyway, if you zoom the viewer out (mouse wheel) then you should then see a monoclinic box of silica!

* * *
{{< button pos="left" text="Previous Step" path="step2/">}}
{{< button pos="right" text="Next Step" path="step4/">}}
