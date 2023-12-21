---
title: Step 6 - Fix the Water Molecule Geometry
type: docs
weight: 8
---


There is a hint in the structure factors for the H<sub>2</sub>O sample (particularly the G(r)), that the intramolecular geometry of our off-the-shelf forcefield is not quite consistent with the real world.  This is made clearly obvious if you look at the G(r) for the D<sub>2</sub>O sample:

{{< action type="tabs" >}}Go to the {{< gui-tab text="G(r) / Neutron S(Q)" type="Layer" >}} tab {{< /action >}}
{{< action type="mouse" >}}Click on the "D2O" {{< module "NeutronSQ" >}} module and select the **Output** button{{< /action >}}
{{< action type="mouse" >}}Click the {{< gui-button text="Total G(r)" >}} button on the "H2O" {{< module "NeutronSQ" >}} module's output page{{< /action >}}

{{< cimage src="../equilibrated-d2o-gr.png" caption="Simulated (black), Fourier-transformed from simulated F(Q) (green), and experimental (red) G(r) for the equilibrated water (D<sub>2</sub>O) simulation" >}}

Clearly we have a mismatch between the experimental (red) and simulated (green) peak positions at around 1 &#8491; (related to the O-H bond) and 1.7 &#8491; (related to the H-O-H angle). It is usually necessary to adjust the geometry of your species a little in order to be consistent with the experimentally-measured data, and in the present case we are lucky that we only have two parameters to adjust!

{{< warn text="Here we are modifying the intramolecular terms based on comparison of the D<sub>2</sub>O data, but bear in mind that liquid water is amongst the systems most sensitive to isotopic substitution since all hydrogens are hydroxyl hydrogens, and subject to exchange as well as strong hydrogen bonding. As such, the differences in intramolecular geometry between H<sub>2</sub>O and D<sub>2</sub>O are measurable.<sup>[1]</sup>" />}}

Since we set up our simulation to use intramolecular master terms (via the **Add Forcefield Terms...** wizard) we can modify those to directly affect our water molecule's geometry. For the O&ndash;H bond it is quite straightforward to read of the true distance (0.976 &#8491;) from the reference g(r). The angle distance requires a touch more trigonometry but, given knowledge of the correct O&ndash;H distance, we can work out that the corresponding equilibrium angle we require is 107.134 &deg;.

First of all, let's stop the simulation from running:

{{< action type="menu" text="Simulation &#8680; Pause" />}}
...or...
{{< action type="key" text="Esc" />}}

And now let's make the changes to our intramolecular terms:

{{< action type="tabs" >}}Go to the {{< gui-tab text="Forcefield" type="ff" >}} tab and find the **Master Terms** section{{< /action >}}
{{< action type="edit" text="Change the equlibrium bond length ('eq') of the `HW-OW` bond term from 1.0 to 0.976 &#8491;" />}}
{{< action type="edit" text="Change the equilibrium angle ('eq') of the `HW-OW-HW` angle term from 113.24 to 107.134 &deg;" />}}

Now run the simulation for a little longer and let the species adjust to their new geometry, and you should see a marked improvement in the comparison of the D<sub>2</sub>O total G(r) and structure factor.

{{< cimage src="../equilibrated-d2o-adjusted-gr.png" caption="Simulated (black), Fourier-transformed from simulated F(Q) (green), and experimental (red) G(r) for the equilibrated water (D<sub>2</sub>O) simulation with adjusted intramolecular parameters" >}}

{{< tip >}}The change in the G(r) will not be instant as the majority of the evolution of the system is from the {{< module "MolShake" >}} module which does not change the intramolecular geometry. Only the {{< module "MD" >}} module will affect the intramolecular geometry. Also, the g(r) calculated by the {{< module "GR" >}} are averaged over five calculations by default. {{< /tip >}}

It's also worth checking the other two samples, where the same kind of improvement should be noticeable (if a little less prominent).

### References
1. [Quantum Differences between Heavy and Light Water](https://journals.aps.org/prl/abstract/10.1103/PhysRevLett.101.065502), A. K. Soper and C. J. Benmore, Phys. Rev. Lett. **101**, 065502 (2008).

* * *
{{< button pos="left" text="Previous Step" path="step5/">}}
{{< button pos="right" text="Next Step" path="step7/">}}
