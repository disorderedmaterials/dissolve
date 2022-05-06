---
title: Step 5 - Equilibrate the System
type: docs
---

With our two processing layers set up we can start to run the simulation and monitor how the system is evolving. We'll do this from the {{< gui-button text="Output" icon="general_output" >}} page of the {{< gui-module "NeutronSQ" >}} module.

{{< action type="tabs" >}}Go to the {{< gui-tab type="Layer" text="RDF and Neutron S(Q)" >}} layer tab{{< /action >}}
{{< action type="groups" >}}Select the {{< gui-module "NeutronSQ" >}} module and open the {{< gui-button text="Output" icon="general_output" >}} page{{< /action >}}
{{< step text="We'll visually compare the simulated total structure factor to the experimental one" />}}
{{< action type="menu" text="Simulation &#8680; Run" />}}
{{< step text="You can also use `Ctrl-R` to start a simulation running" />}}

{{< tip text="Note the counter towards the right-hand side of the status bar at the bottom of the main window which tracks the current iteration, and the status indicator to the far left of the status bar telling you what Dissolve is doing (and also note that pressing `Esc` stops the current simulation)." >}}

While the simulation is running you cannot edit any input values, keywords etc., but you can investigate the simulation's progress and output as it happens. For example, you could go to the {{< gui-tab type="Layer" text="Standard Atomic (MC/MD)" >}} evolution layer and look at the {{< gui-button text="Output" icon="general_output" >}} page of the {{< gui-module "Energy" >}} module to see what the total energy of the configuration is doing.

After the simulation has been running for a little while (perhaps 100 iterations), you'll see that the calculated data compare quite favourably with the reference data, with the G(r) and F(Q) looking something like this:

{{< cimage src="../equilibrated-gr.png" caption="Equilibrated total G(r) for liquid argon" >}}
{{< cimage src="../equilibrated-fq.png" caption="Equilibrated total F(Q) for liquid argon" >}}

When there is no further dramatic change in the calculated data you can stop the simulation:

{{< action type="menu" text="Simulation &#8680; Stop" />}}
{{< step text="Or..." />}}
{{< action type="key" text="Esc" />}}

Keep in mind that the simulation will not actually stop until the current iteration is completed - most parts of the GUI will remain grayed out until then.

* * *
{{< button pos="left" text="Previous Step" path="step4b/">}}
{{< button pos="right" text="Next Step" path="step6/">}}
