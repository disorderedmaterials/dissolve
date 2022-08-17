---
title: Step 1 - Set up the Species
type: docs
weight: 1
---

### Create the Species

We can import the two species we need from some xyz datafiles supplied in the example data:

{{< action type="menu" text="Species &#8680; Import &#8680; From XYZ..." />}}
{{< step text="Choose the `ipa.xyz` file." />}}

{{< action type="menu" text="Species &#8680; Import &#8680; From XYZ..." />}}
{{< step text="Choose the `water.xyz` file." />}}

Each will need to have a suitable forcefield applied since Dissolve demands that, at the very least, atom types have been assigned to all atoms. The forcefield doesn't have to be "complete" in the sense of intramolecular terms, torsions, etc.

So, apply perhaps the `SPC/Fw` model to the water, and either `OPLS-AA/Alcohols` or `UFF` to the IPA.

### Create Analysis Sites

How many analysis sites you define will of course depend on what you want to calculate - if, indeed, you are calculating anything at all (beyond using the standard correlation function modules). For the sake of example we'll calculate some centre-of-mass RDFs here, so you'll need a centre-of-mass site defined for each of the two species.  If you want to define more than that, or add orientation to the centre-of-mass sites, etc., please feel free to do so!

* * *
{{< button pos="left" text="Back to Overview" path="">}}
{{< button pos="right" text="Next Step" path="step2/">}}
