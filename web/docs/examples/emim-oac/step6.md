---
title: Step 6 - Initial Comparison
type: docs
weight: 1
---

Now that we have our structure factor calculations set up let's run for a few steps and see what they look like.

{{< action type="key" >}}Ctrl-5{{< /action >}}

You can go to the {{< gui-button text="Output" icon="output" >}} tab of any of the {{< module "NeutronSQ" >}} modules and see the comparison of the F(Q) - we re-plot all seven together for convenience below (this is output from the {{< module "EPSR" >}} module):

{{< cimage src="../equilibrated-fq.png" caption="Calculated total structure factors (dashed lines) compared to the experimental data (solid lines) in the equilibrated simulation" >}}

Overall the calculate F(Q) agree rather well with the experimental curves, with the exception that the peak at Q = 1.5 &#8491;<sup>-1</sup> is too intense and well-defined, particularly for the `H3D8-D` and `H3D8-D` samples. This peak is related to ion-ion neighbour interactions - we can lightly abuse an equation related to Bragg scattering, $r = 2 \pi / Q$, to calculate the real-space correlation distance as 4.19 &#8491; - and so suggests that the forcefield, even with the scaled charges, is giving a liquid that is overstructured from too-strong interactions between the cation and anion.

We'll let the {{< module "EPSR" >}} module sort that out for us!

{{< action type="menu" text="Layer &#8680; Create &#8680; Refinement &#8680; Standard EPSR" />}}

Because of the stronger intermolecular interactions manifesting in a system of ions we will need to push the refinement a bit harder than usual. You can run the refinement with the default value of **EReq** of 3, but you will see very little improvement in the structure factors. Standard practice is to then sequentially increase this value in small steps (e.g. increments of 3 at a time) until no further improvement to the fits (as measured by the total R-factor) can be achieved. Simply putting in a huge number that is "definitely enough" is not a good idea - experimental data is imperfect to some degree, and at some point the generated potentials are highly likely to force the system into a worse state than it was before.

For the present system a value of about 17 is optimal, and we'll put this in straight away:

{{< action type="tabs">}}Go to the {{< gui-tab type="layer" text="Refine (EPSR)">}} layer tab{{< /action >}}
{{< action type="mouse">}}Select the {{< module "EPSR" >}} module{{< /action >}}
{{< action type="edit">}}In the module {{<gui-button icon="options" text="Options">}} change the **EReq** value to `17.0`{{</action>}}
{{< action type="mouse">}}Click the {{< gui-button icon="output" text="Output" >}} tab{{< /action >}}

Running the simulation for another 1000 iterations should give us a good refined structure:

{{< action type="key" >}}Ctrl-F{{< /action >}}
{{< step >}}Set the number of iterations  to `1000` and hit {{< gui-button text="OK" >}}{{< /step >}}

* * *
{{< button pos="left" text="Previous Step" path="step5/">}}
{{< button pos="right" text="Next Step" path="step7/">}}
