---
title: Step 4 - Equilibrate the Box
parent: Bulk Silica
grand_parent: Examples
---

## 4. Equilibrate the Box

To ensure that our silicon and oxygen atoms are well mixed and distributed we're going to "cook" the box at a high temperature to start with.

> **Bulk** configuration tab
{: .action .action_tabs}
> Change the **Temperature** to 2000.0
{: .action .action_edit}

Now we'll run for 250 steps

> Simulation &#8680; Run For...
{: .action .action_menu}
> Set the number of steps to 250 and click _OK_
{: .step}

You can open (double-click) the [`NeutronSQ`](../../userguide/modules/neutronsq) module in the _RDF / Neutron S(Q)_ layer to monitor the structure, or the [`Energy`](../../userguide/modules/energy) module in the _Evolve (Basic Atomic)_ to check the energy as the simulation progresses. The total energy will be large and negative - of the order of &ndash;4&mult;10<sup>6</sup>. When finished, the F(Q) and G(r) should look a little like this:

![](cooked-fq.png)
_Calculated structure factor (black line) of amorphous silica at high temperature (2000 K) compared to reference data at 298 K (red line)_

![](cooked-gr.png)
_Calculated total G(r) (black line) of amorphous silica at high temperature (2000 K) compared to Fourier transform of reference data at 298 K (red line)_

We'll now reset the temperature of the box to 298 K and run a second equilibration.

> **Bulk** configuration tab
{: .action .action_tabs}
> Change the **Temperature** to 298.0
{: .action .action_edit}
> Simulation &#8680; Run For...
{: .action .action_menu}
> Set the number of steps to 250 and click _OK_
{: .step}

Once finished, the structure should look a lot better:

![](cooked-fq.png)
_Equilibrated structure factor (black line) of amorphous silica compared to reference data at 298 K (red line)_

![](cooked-gr.png)
_Equilibrated total G(r) (black line) of amorphous silica compared to Fourier transform of reference data at 298 K (red line)_

[Previous Step](/docs/examples/silica_bulk/step3){: .btn }   [Next Step](/docs/examples/silica_bulk/step5){: .btn .right}
