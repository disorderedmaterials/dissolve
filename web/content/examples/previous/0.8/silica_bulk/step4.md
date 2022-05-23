---
title: Step 4 - Equilibrate the Box
type: docs
---


To ensure that our silicon and oxygen atoms are well mixed and distributed we're going to "cook" the box at a high temperature to start with.

{{< action type="tabs" text="**Bulk** configuration tab" >}}
{{< action type="edit" text="Change the **Temperature** to 2000.0" >}}


Now we'll run for 250 steps

{{< action type="menu" text="Simulation &#8680; Run For..." >}}
{{< step text="Set the number of steps to 250 and click _OK_" >}}

You can open (double-click) the [`NeutronSQ`]({{< ref "neutronsq" >}}) module in the _RDF / Neutron S(Q)_ layer to monitor the structure, or the [`Energy`]({{< ref "energy" >}}) module in the _Evolve (Basic Atomic)_ to check the energy as the simulation progresses. The total energy will be large and negative - of the order of &ndash;4&mult;10<sup>6</sup>. When finished, the F(Q) and G(r) should look a little like this:

{{< cimage src="../cooked-fq.png" caption="Calculated structure factor (black line) of amorphous silica at high temperature (2000 K) compared to reference data at 298 K (red line)" >}}

{{< cimage src="../cooked-gr.png" caption="Calculated total G(r) (black line) of amorphous silica at high temperature (2000 K) compared to Fourier transform of reference data at 298 K (red line)" >}}

We'll now reset the temperature of the box to 298 K and run a second equilibration.

{{< action type="tabs" text="**Bulk** configuration tab" >}}
{{< action type="edit" text="Change the **Temperature** to 298.0" >}}
{{< action type="menu" text="Simulation &#8680; Run For..." >}}
{{< step text="Set the number of steps to 250 and click _OK_" >}}


Once finished, the structure should look a lot better:

{{< cimage src="../cooked-fq.png" caption="Equilibrated structure factor (black line) of amorphous silica compared to reference data at 298 K (red line)" >}}

{{< cimage src="../cooked-gr.png" caption="Equilibrated total G(r) (black line) of amorphous silica compared to Fourier transform of reference data at 298 K (red line)" >}}

{{< button pos="left" text="Previous Step" path="step3/">}}
{{< button pos="right" text="Next Step" path="step5/">}}
