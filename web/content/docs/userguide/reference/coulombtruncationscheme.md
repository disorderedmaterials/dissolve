---
title: Coulomb Truncation Schemes
description: Available truncation schemes for coulomb interactions
---

|Keyword|Parameters|Description|
|:---:|:--------:|-----------|
|`None`|--|No truncation scheme|
|`Shifted`|--|The charge interaction is shifted such that, at $r = r_{max}$, the contribution to the energy and force is zero $$ E^{shifted}_{q_iq_j} = q_i q_j \left( \frac{1}{r} + \frac{r}{r^2_{max}} - \frac{2}{r_{max}} \right) $$ $$ F^{shifted}_{q_iq_j} = q_i q_j \left( \frac{1}{r^2} - \frac{1}{r^2_{max}} \right) $$ |
