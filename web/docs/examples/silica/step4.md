---
title: Step 4 - Set up Processing
type: docs
---


Let's create two processing layers now - one to evolve our configuration, and one to calculate the current RDFs and structure factors.

We have only atomic species, so we'll choose the basic atomic evolution layer for our system:

{{< action type="menu" text="Layer &#8680; Create &#8680; Evolution &#8680; Atomic (MC only)" />}}

Our reference data is neutron-weighted so:

{{< action type="menu" text="Layer &#8680; Create &#8680; Correlations &#8680; RDF and Neutron S(Q)" />}}

We just need to set the instrumental _Q_ broadening in the {{< module "SQ" >}} module and assign the reference data to our {{< module "NeutronSQ" >}} module.

First, the Q-broadening:

{{< action type="mouse" >}}Click on the {{< module "SQ" >}} module to display its {{< gui-button text="Options" icon="general_options" >}}{{< /action >}}
{{< action type="edit" text="Set the **QBroadening** to `OmegaDependentGaussian` with a FWHM of `0.03`" />}}

Now for the reference data. Dissolve can perform some rudimentary operations on datafiles when reading them in - for our present experimental reference data ("SiO2_NIMROD.mint01") it contains a point at _Q_ = 0.0 &#8491;<sup>-1</sup> which we will trim off by setting the minimum x (Q) value to read.

{{< action type="mouse" >}}Click on the {{< module "NeutronSQ" >}} module to display its options{{< /action >}}
{{< action type="edit" text="For the **Reference** keyword in the **Reference Data** settings group, select the file `SiO2_NIMROD.mint01` and set the format of the data to `mint`" />}}
{{< step >}}Open the settings for the **Reference** datafile by clicking the {{< gui-button icon="general_options" text="Options" >}} button near to the file type selector, and in the **Manipulations** tab set _XMin_ to `0.1`{{< /step >}}


* * *
{{< button pos="left" text="Previous Step" path="step3/">}}
{{< button pos="right" text="Next Step" path="step5/">}}
