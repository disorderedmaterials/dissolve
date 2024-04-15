---
title: Step 2 - Set up Atomic Species
type: docs
---

Now let's generate our atomic species, assign the atom types that we created in the last step, and create a basic site for each so we can do some analysis afterwards. First, the silicon:

{{< action type="menu" text="Species &#8680; Create &#8680; Atomic..." />}}
{{< step text=" Select silicon from the periodic table" />}}
{{< action type="edit">}}In the {{<gui-tab text="Atoms" type="species">}} section, change the _AtomType_ (AT) for the atom to `Si`{{</action>}}
{{< action type="mouse">}}With the atom selected in the table, right click the atom in the viewer and click {{< gui-button text="Create site from" >}} &#8680; {{< gui-button text="Atoms" >}}{{</action>}}

And now, the oxygen:

{{< action type="menu" text="Species &#8680; Create &#8680; Atomic..." />}}
{{< step text="Select oxygen from the periodic table" />}}
{{< action type="tabs">}}Go to the {{<gui-tab type="ff" text="Forcefield">}} tab{{</action>}}
{{< action type="edit">}}In the {{<gui-tab text="Atoms" type="species">}} section, change the _AtomType_ (AT) for the atom to `O`{{</action>}}
{{< action type="mouse">}}With the atom selected in the table, right click the atom in the viewer and click {{< gui-button text="Create site from" >}} &#8680; {{< gui-button text="Atoms" >}}{{</action>}}


* * *
{{< button pos="left" text="Previous Step" path="step1/">}}
{{< button pos="right" text="Next Step" path="step3/">}}
