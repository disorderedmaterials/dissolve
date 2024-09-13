---
title: TR (Module)
linkTitle: TR
description: Calculate total correlation functions between all atom types
---

## Overview

The `TR` module calculates a full set of partial total distribution functions, $t_{ij}(r)$ for each unique pair of atom types $i$ and $j$ present in the target configuration(s).

The `TR` module uses equations from Keen, D. A. (2001) and the numbered equations are referenced from there.

## Description

For a configuration containing $N$ distinct atom types there are $\frac{1}{2}N(N+1)$ unique partial total distribution functions $t_{ij}(r)$, where $i$ and $j$ are the $i^{th}$ and $j^{th}$ atom type respectively. The standard formalism for a total distribution function between atom types $i$ and $j$ is

$$ T_{ij}(r) = \sum^n_{i,j}c_i b_i b_j t_{ij}(r)$$ (31)

Where t_{ij}(r) is calucluated via:

$$ t_{ij}(r) = 4 /pi r c_j /rho_0 g_{ij} $$ (33)

## Note
It is reccomended to compare the partials to the total that the user scales them via eqution 31.
