---
title: Averaging Scheme
description: Averaging methods for numerical quantities
---

|Keyword|Parameters|Description|
|:---:|:--------:|-----------|
|`Linear`|--|Each data point has equal weight in the averaging $$ \bar{\chi} = \frac{1}{N}\sum_{i=1}^N \chi_i $$ |
|`Exponential`|--|Each data point is weighted according to a decreasing exponential scheme $$ \bar{\chi} = \frac{1 - \alpha^N}{1 - \alpha} \sum_{i=1}^N \alpha^i \chi_i $$ where $\alpha$ is the exponential decay factor (default = 0.7)|
