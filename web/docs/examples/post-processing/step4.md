---
title: Step 4 - RDF and Structure Factors
type: docs
weight: 1
---

Let's create our correlation layer:

{{< action type="menu" text="Layer &#8680; Create &#8680; Correlations &#8680; RDF and Neutron S(Q)" />}}

Recall that our {{< module "ImportTrajectory" >}} module is running every iteration, so our correlation calculation layer needs to do the same:

{{< action type="tabs" >}}Go to the {{< gui-tab type="layer" text="G(r) / Neutron S(Q)" >}} layer tab{{< /action >}}
{{< action type="edit" >}}Change the frequency in the _Layer Control_ at the top-left of the tab from `5` to `1`{{< /action >}}

Let's do a quick, single iteration to make sure everything is working as expected:

{{< action type="key" >}}Ctrl-1{{< /action >}}

What will have happened in this iteration is that a single frame will have been read in from the trajectory into the configuration and, if we've got everything else set up correctly, from which some sensible-looking radial distribution functions and (neutron-weighted) structure factors will have been calculated. 

{{< action type="groups" >}}Go to the {{< module "NeutronSQ" >}} module and open its {{<gui-button icon="output" text="Output">}} tab{{< /action >}}

The total F(Q) should look like this:

{{< cimage src="../first-fq.png" caption="Total neutron-weighted structure factor from the first frame of the reference trajectory" >}}

* * *
{{< button pos="left" text="Previous Step" path="step3/">}}
{{< button pos="right" text="Next Step" path="step5/">}}
