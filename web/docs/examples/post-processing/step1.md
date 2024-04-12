---
title: Step 1 - Set up the Species
type: docs
weight: 1
---

### Create the Species

We can import the two species we need from some xyz datafiles supplied in the example data:

{{< action type="menu" text="Species &#8680; Import &#8680; From XYZ..." />}}
{{< step text="Choose the `ipa.xyz` file." />}}
{{< action type="edit">}}Change the name to `IPA` by double clicking the {{<gui-tab type="species" text="NewSpecies">}} tab{{</action>}}
{{< action type="menu" text="Species &#8680; Add Forcefield Terms..." />}}
{{< step >}} From the available forcefields choose `OPLS-AA/Alcohols` or `UFF` (your choice!) and click {{< gui-button text="Next" icon="arrowright" >}} {{< /step >}}
{{< step >}}Leave all other settings as default and click {{< gui-button text="Next" icon="arrowright" >}} until finished{{< /step >}}

Now for the water:

{{< action type="menu" text="Species &#8680; Import &#8680; From XYZ..." />}}
{{< action type="edit">}}Change the name to `water` by double clicking the {{<gui-tab type="species" text="NewSpecies">}} tab{{</action>}}
{{< action type="menu" text="Species &#8680; Import &#8680; From XYZ..." />}}
{{< step >}} From the available forcefields choose `SPC/Fw` and click {{< gui-button text="Next" icon="arrowright" >}} {{< /step >}}
{{< step >}}Leave all other settings as default and click {{< gui-button text="Next" icon="arrowright" >}} until finished{{< /step >}}

Each species needs to have a suitable forcefield applied since Dissolve demands that, at the very least, atom types to be assigned to all atoms.

### Create Analysis Sites

How many analysis sites you define will of course depend on what you want to calculate - if, indeed, you are calculating anything at all (beyond using the standard correlation function modules). For the sake of example we'll calculate some centre-of-mass RDFs here, so you'll need a centre-of-mass site defined for each of the two species.  If you want to define more than that, or add orientation to the centre-of-mass sites, etc., please feel free to do so!

* * *
{{< button pos="left" text="Back to Overview" path="">}}
{{< button pos="right" text="Next Step" path="step2/">}}
