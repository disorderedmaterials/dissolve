---
title: Timestep Type
description: Timestep strategy for molecular dynamics
---

|Keyword|Parameters|Description|
|:---:|:--------:|-----------|
|`Fixed`|--|Use a pre-defined fixed timestep|
|`AverageSquared`|--|Use the variable timestep strategy of Marks and Robinson.[1] This algorithm is robust but tends to give too-small timesteps for molecular (bound) systems|
|`Auto100`|--|Use a custom algorithm which scales the timestep based on the maximal interatomic force only (i.e. that arising from pair potential contributions), and which uses a specified fixed timestep as its limiting maximum|

[1] "Variable timestep algorithm for molecular dynamics simulation of non-equilibrium processes", N. A. Marks and M. Robinson, _Nucl. Instrum. Meth. B_ **352**, 3 (2015), [10.1016/j.nimb.2014.11.094](https://doi.org/10.1016/j.nimb.2014.11.094)
