---
title: TR (Module)
linkTitle: TR
description: Calculate total correlation functions between all atom types
---

## Overview

The `TR` module calculates a full set of partial total distribution functions, $t_{ij}(r)$ for each unique pair of atom types $i$ and $j$ present in the target configuration(s).

The `TR` module uses equations from [Keen, D. A. (2001)](https://journals.iucr.org/paper?S0021889800019993) and the numbered equations are referenced from there.

## Description

For a configuration containing $N$ distinct atom types there are $\frac{1}{2}N(N+1)$ unique partial total distribution functions $t_{ij}(r)$, where $i$ and $j$ are the $i^{th}$ and $j^{th}$ atom type respectively. The standard formalism for a total distribution function between atom types $i$ and $j$ is

$$ T_{ij}(r) = \sum^n_{i,j}c_i b_i b_j t_{ij}(r)  (31)$$

Where $t_{ij}(r)$ is calculated via:

$$ t_{ij}(r) = 4 \pi r c_j \rho_0 g_{ij}  (33)$$

## Note
The partial $t(r)$ that are output from the module do not include the scaling before the summation is done. It is recommended for comparison of the partials to the total $T(R)$ that the user scales them via equation 31. This includes multiplying the partial output from Dissolve by $c_i$, $b_i$ and $b_j$.
