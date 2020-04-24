---
title: Step 3 - Set up Processing
grand_parent: Examples
---

## 3. Set up Processing

Let's create two processing layers now - one to evolve our configuration, and one to calculate the current RDFs and structure factors.

We have only atomic species, so we'll choose the basic atomic evolution layer for our system:

> Layer &#8680; Create... &#8680; Evolution... &#8680; Atomic (MC only)
{: .action .action_menu}

Our reference data is neutron-weighted so:

> Layer &#8680; Create... &#8680; Calculate... &#8680; RDF and Neutron S(Q)
{: .action .action_menu}

We just need to set the instrumental _Q_ broadening and add the reference data to our [`NeutronSQ`](../../userguide/modules/neutronsq) module:

> Click on the [`NeutronSQ`](../../userguide/modules/neutronsq) module to display its options
{: .action .action_mouse}
> Open the **Calculation** settings group
{: .action .action_groups}
> Set the **QBroadening** to `OmegaDependentGaussian` with a FWHM of 0.03
{: .step}
> Open the **Reference Data** settings group
{: .action .action_groups}
> For the **Reference** keyword select the file `SiO2_NIMROD.mint01` and set the format of the data to `mint`
{: .step}
> The datafile contains a point at _Q_ = 0.0 which we must remove, so tick the **ReferenceIgnoreFirst** option
{: .step}

[Previous Step](/docs/examples/silica_bulk/step2/){: .btn }   [Next Step](/docs/examples/silica_bulk/step4/){: .btn .right}
