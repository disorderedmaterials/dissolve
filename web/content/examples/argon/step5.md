---
title: Step 5 - Equilibrate the System
type: docs
---

With two of our processing layers set up we can start to run the simulation and monitor how the system is evolving. There are several ways to run the simulation within the Dissolve GUI.

{{< action type="mouse" text="Click the **Run** button on the left-hand side of the main window" >}}
{{< action type="menu" text="Simulation &#8680; Run" >}}
{{< action type="key" text="Ctrl-R" >}}

Do one of those to set the simulation running. Note the counter in the top-left of the main window which tracks the current iteration.

While the simulation is running you cannot edit any input values, keywords etc., but you can investigate the simulation's progress and output as it happens. For example, you could go to your _Standard Atomic (MC/MD)_ evolution layer and double-click the [`Energy`]({{< ref "energy" >}}) module to see a plot of the total energy of the configuration.

Now go back to your _RDF and Neutron S(Q)_ layer and select the [`NeutronSQ`]({{< ref "neutronsq" >}}) module - here you can see how the calculated structure factor and total G(r) compare to the experimental data we loaded in. After the simulation has been running for a little while (perhaps 100 iterations), you'll see that the simulation and experiment actually compare quite favourably, with the G(r) and F(Q) looking something like this:

{{< cimage src="../equilibrated-gr.png" caption="Equilibrated total G(r) for liquid argon" >}}
{{< cimage src="../equilibrated-fq.png" caption="Equilibrated total F(Q) for liquid argon" >}}

We'll now stop the simulation.

{{< action type="mouse" text="Click the **Pause** button on the left-hand side of the main window" >}}
{{< action type="menu" text="Simulation &#8680; Pause" >}}
{{< action type="key" text="Esc" >}}

Keep in mind that the simulation will not actually stop until the current iteration is completed - most parts of the GUI will remain grayed out until then.

{{< button pos="left" text="Previous Step" path="step4b/">}}
{{< button pos="right" text="Next Step" path="step6/">}}
