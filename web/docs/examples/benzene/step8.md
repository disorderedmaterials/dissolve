---
title: Step 8 - Calculate Refined Properties
type: docs
---


We now have our refined simulation that is in better agreement with the experimental data, so its time to recalculate our properties so that we can compare them to those we saved earlier. All the processing layers should now be enabled and remain on for the production run, including the refinement layer.

{{< action type="tabs">}}Go to the {{< gui-tab type="layer" text="Analysis">}} layer tab{{< /action >}}
{{< action type="check" text="Enable the layer by clicking the ![](/img/slider-off.png) button in the _Layer Control_ section so it displays ![](/img/slider-on.png)" />}}
{{< action type="menu" text="Simulation &#8680; Run For..." />}}
{{< action type="edit">}}Enter 1000 and click {{< gui-button text="OK" >}}{{< /action >}}

If you're ready for another drink, now would be another opportune moment, or perhaps there's another pressing email that needs hammering out?

Once those iterations have completed, we can move on to do the comparison of the data. Also, we can snapshot our production run data:

{{< action type="menu" text="File &#8680; Save Restart Point..." />}}
{{< action type="edit">}}Change the filename to `production.restart` and click {{< gui-button text="Save" >}}{{< /action >}}


* * *
{{< button pos="left" text="Previous Step" path="step7/">}}
{{< button pos="right" text="Next Step" path="step9/">}}
