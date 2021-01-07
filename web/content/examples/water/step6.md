---
title: Step 6 - Fix the Water Molecule Geometry
type: docs
weight: 8
---


There is a hint in the structure factors for the H<sub>2</sub>O sample (particularly the G(r)), that the intramolecular geometry of our off-the-shelf forcefield is not quite consistent with the real world.  This is made clearly obvious if you look at the G(r) for the D<sub>2</sub>O sample:

{{< action type="tabs" text="**RDF / Neutron S(Q)** tab" >}}
{{< step text="Click on the `D2O` `NeutronSQ` module and select the **Output** button" >}}


{{< cimage src="../equilibrated-d2o-broadened-gr.png" caption="Equilibrated water (D2O) G(r) with effective broadening applied to intramolecular g(r)" >}}

{{< warn text="You may notice a small 'upturn' in the calculated F(Q) for the D<sub>2</sub>O sample that does not match experiment. This is caused by subtle density fluctuations in the configuration that are accentuated by the deuteration of the water, and will disappear slowly over time. You can safely proceed with the example." >}}

Clearly we have a mismatch between the peak positions at around 1 &#8491; (related to the O-H bond) and 1.7 &#8491; (related to the H-O-H angle). It is usually necessary to adjust the geometry of your species a little in order to be consistent with the experimentally-measured data, and in the present case we are lucky that we only have two parameters to adjust!

{{< warn text="Here we are modifying the intramolecular terms based on comparison of the D<sub>2</sub>O data, but bear in mind that liquid water is amongst the systems most sensitive to isotopic substitution since all hydrogens are hydroxyl hydrogens, and subject to exchange as well as strong hydrogen bonding. As such, the differences in intramolecular geometry between H<sub>2</sub>O and D<sub>2</sub>O are measurable.<sup>[1]</sup>" >}}

Since we set up our simulation to use intramolecular master terms (via the **Add Forcefield Terms...** wizard) we can modify those to directly affect our water molecule's geometry. For the O&ndash;H bond it is quite straightforward to read of the true distance (0.976 &#8491;) from the reference g(r). The angle distance requires a touch more trigonometry but, given knowledge of the correct O&ndash;H distance, we can work out that the corresponding equilibrium angle we require is 107.134 &deg;.

{{< action type="tabs" text="**Forcefield** tab, **Master Terms** section" >}}
{{< step text="Change the bond length (_Parameter 2_) of the `HW-OW` bond term from 1.0 to 0.976 &#8491;" >}}
{{< step text="Change the equilibrium angle (_Parameter 2_) of the `HW-OW-HW` angle term from 113.24 to 107.134 &deg;" >}}

Now run the simulation for a little longer and let the species adjust to their new geometry, and you should see a marked improvement in the comparison of the D<sub>2</sub>O total G(r) and structure factor.

{{< cimage src="../equilibrated-d2o-broadened-adjusted-gr.png" caption="Equilibrated water (D2O) G(r) with effective broadening and adjusted intramolecular geometry" >}}

{{< tip text="The change in the G(r) will not be instant as the majority of the evolution of the system is from the `MolShake` which does not change the intramolecular geometry. Only the `MD` module will affect the intramolecular geometry. Also, the g(r) calculated by the `RDF` are averaged over five calculations by default." >}}

It's also worth checking the other two samples, where the same kind of improvement should be noticeable (if a little less prominent).

### References
1. [Quantum Differences between Heavy and Light Water](https://journals.aps.org/prl/abstract/10.1103/PhysRevLett.101.065502), A. K. Soper and C. J. Benmore, Phys. Rev. Lett. **101**, 065502 (2008).

{{< button pos="left" text="Previous Step" path="step5/">}}
{{< button pos="right" text="Next Step" path="step7/">}}
