---
title: Normalisation Type
description: Normalisation types for total structure factors
---

|Keyword|Parameters|Description|
|:---:|:--------:|-----------|
|`None`|--|No normalisation|
|`AverageSquared`|--|Normalise to the average of the squares of the bound coherent scattering lengths $b_i$ of all atom types $i$, where the atomic fractions are $c_i$ $$ \alpha = \sum^N_{i=1} c_i b^2_i $$ |
|`SquaredAverage`|--|Normalise to the square of the average bound coherent scattering length $b_i$ of all atom types $i$, where the atomic fractions are $c_i$ $$ \alpha = \left( \sum^N_{i=1} c_i b_i \right)^2 $$ |
