---
title: Step 3 - Import Reference Data
type: docs
weight: 1
---

At this point we would normally add an evolution layer in order to move our configuration contents around, but here we'll import a trajectory instead.

{{< action type="menu" text="Layer &#8680; Create... &#8680; Empty" />}}
{{< action type="edit">}}Double-click the layer tab and rename it to {{<gui-tab type="layer" text="Import">}}{{</action>}}
{{< action type="mouse">}}Show the module palette for the current layer by clicking the {{< gui-button icon="palette" text="Show Available Modules">}} button at the bottom of the module list on the left{{< /action >}}
{{< action type="mouse">}}Drag an {{< module "ImportTrajectory" >}} module from the _Import_ section into the layer{{< /action >}}

An important thing to note here is that the {{< module "ImportTrajectory" >}} module should run every iteration - otherwise, there is the risk of averaging calculated quantities over identical configuration snapshots in the trajectory and polluting your results.

{{< action type="edit">}}Set the target file of the {{< module "ImportTrajectory" >}} module to the `9molpcnt-IPA-water.xyz` trajectory{{</action>}}

Now we're ready to add some calculation layers. Note that the {{< module "ImportTrajectory" >}} has a configuration target just as many other modules do. Here it is the target configuration whose coordinates we are going to overwrite.

* * *
{{< button pos="left" text="Previous Step" path="step2/">}}
{{< button pos="right" text="Next Step" path="step4/">}}
