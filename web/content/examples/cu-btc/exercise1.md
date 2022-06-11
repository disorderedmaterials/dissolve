---
title: Exercise 1 - Better Total G(r)
type: docs
weight: 101
---

We saw how to improve agreement between the experiment and simulation by modifying the equilibrium distance of the Cu&ndash;O master bond in the forcefield. How much better can we make the agreement?

Here we'll focus on the peak around 2.8 &#8491;, whose assignment is as follows:

|Experimental $r$, &#8491;|Simulated $r$, &#8491;|(Possible) Peak Assignment|
|:----------:|:--------:|---------|
|2.77|2.83|C(3)&ndash;O (proximity)|

The peak appears to be associated to the proximity of the C(3) and O atoms in the structure, and which essentially are connected by a torsion interaction C3&ndash;C2&ndash;C1&ndash;O. The central interaction C2&ndash;C1 is the bond connecting the carboxylate group to the benzene ring. So how can we improve (reduce) the distance between the C3 and O atoms at the termini of this interaction?

{{< tip >}}One possibility is to change the equilibrium bond distance of the middle C2&ndash;C1 in order to push the C3 and O atoms apart. But what effect will this have on other parts of the structure as represented by the G(r) and, indeed, the F(Q)?{{< /tip >}}

* * *
{{< button pos="left" text="Back to Exercises Index" path="exercises/">}}
