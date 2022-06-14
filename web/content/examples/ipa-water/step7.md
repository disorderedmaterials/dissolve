---
title: Step 7 - Set Up Potential Refinement
type: docs
weight: 1
---

Now we are on our way to beginning the refinement, but we first need to set-up the refinement layer.

{{< action type="menu" text="Layer &#8680; Create &#8680; Refinement &#8680; Standard EPSR" />}}

Our new layer contains only the {{< gui-module "EPSR" >}} module, and which Dissolve has set up with sensible targets and defaults. We have previously gone into detail(link) about the refinement stage, for now we will stick to using the majority of the default parameters. However, we are going to use the an ereq of 15.0 for this refinement:

{{< action type="tabs" >}}Go to the {{< gui-tab text="Refine (EPSR)" type="Layer" >}} tab{{< /action >}}
{{< action type="groups">}}Open the **Control** settings group of the {{< gui-module "EPSR" >}} module {{< /action >}} {{< step text=" Change the value of **EReq** to (15.0)" />}}

Before we set the simulation running again, it's advisable to save both the input file and a restart file - just to be on the safer side!

{{< action type="menu" text="File &#8680; Save as..." />}}
{{< action type="edit">}}Change the filename to `preereq.input` and click {{< gui-button text="Save">}}{{< /action >}}

{{< action type="menu" text="File &#8680; Save Restart Point..." />}}
{{< action type="edit">}}Change the filename to `preereq.restart` and click {{< gui-button text="Save">}}{{< /action >}}

We can now start the simulation running again to begin the refinement process. A suitable amount of iterations would once again be in the 1000 region, once you have this started you may want to switch to another task or take a quick break.
