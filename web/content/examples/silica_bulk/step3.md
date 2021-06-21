---
title: Step 3 - Set up Processing
type: docs
---


Let's create two processing layers now - one to evolve our configuration, and one to calculate the current RDFs and structure factors.

We have only atomic species, so we'll choose the basic atomic evolution layer for our system:

{{< action type="menu" text="Layer &#8680; Create... &#8680; Evolution... &#8680; Atomic (MC only)" >}}

Our reference data is neutron-weighted so:

{{< action type="menu" text="Layer &#8680; Create... &#8680; Calculate... &#8680; RDF and Neutron S(Q)" >}}

We just need to set the instrumental _Q_ broadening and add the reference data to our [`NeutronSQ`]({{< ref "neutronsq" >}}) module:

{{< action type="mouse" text="Click on the `NeutronSQ` module to display its options" >}}
{{< action type="groups" text="Open the **Calculation** settings group" >}}
{{< step text="Set the **QBroadening** to `OmegaDependentGaussian` with a FWHM of 0.03" >}}
{{< action type="groups" text="Open the **Reference Data** settings group" >}}
{{< step text="For the **Reference** keyword select the file `SiO2_NIMROD.mint01` and set the format of the data to `mint`" >}}
{{< step text="The datafile contains a point at _Q_ = 0.0 which we will trim off by setting the minimum X value to read in to `0.1`. Open the settings for the **Reference** datafile by clicking the cog button near to the file type selector, and in the **Manipulations** tab set _XMin_ to `0.1`." >}}


{{< button pos="left" text="Previous Step" path="step2/">}}
{{< button pos="right" text="Next Step" path="step4/">}}
