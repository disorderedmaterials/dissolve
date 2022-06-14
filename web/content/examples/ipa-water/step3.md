---
title: Step 3 - Set up Equilibriation
type: docs
weight: 1
---

## The Evolution Layer

We'll use the standard molecular evolution layer for our system:

{{< action type="menu" text="Layer &#8680; Create &#8680; Evolution &#8680; Standard Molecular (MC/MD)" />}}

All of the default values set there will be sufficient.

We can now start running our simulation and start to equilibrate the configuration box.

{{< action type="key" text="Ctrl-R" />}}

It will take a few hundred iterations for the system to equilibrate, we can check this from the graphs of the {{< gui-module "Energy" >}} module, we should see the energy of the system stabilise:

{{< action type="tabs">}}Go to the {{< gui-tab type="layer" text="Evolve (Standard)" >}} layer tab{{< /action >}}
{{< action type="mouse">}}Select the {{< gui-module "Energy" >}} module and go to it's {{< gui-button icon="general_output" text="Output">}} tab{{< /action >}}

There will be some variation of the components, but for a system of 500 molecules the total energy of the system should approach a value of &ndash;24&times;10<sup>3</sup> kJ mol<sup>-1</sup>, with the intermolecular energy around -19x103 kJ mol<sup>-1</sup>.

Once this has been achieved, we can make a snapshot of the current simulation and store this equilibrated point as a backup. The restart file written by Dissolve as it runs stores all the information necessary to continue a stopped simulation, but there is no going back to a previous point in the simulation (e.g. to undo a bad analysis choice, or to reference earlier data) unless we have a suitable restart point to go back to. At any time we can write a new restart file from the GUI, independent of the standard restart file, and which we can keep to load in at a later date.

{{< action type="menu" text="File &#8680; Save Restart Point..." />}}
{{< action type="edit">}}Change the filename to equilibrated.restart and click {{< gui-button text="Save">}}{{< /action >}}

Now we can move on to set up the analysis of the structural properties we're interested in.

{{< button pos="left" text="Previous Step" path="step2/">}} {{< button pos="right" text="Next Step" path="step4/">}}
