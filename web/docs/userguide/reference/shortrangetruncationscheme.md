---
title: Short Range Truncation Schemes
description: Available truncation schemes for short range interactions
---

|Keyword|Parameters|Description|
|:---:|:--------:|-----------|
|`None`|--|No truncation scheme|
|`Cosine`|--|Simple cosine-based truncation, reducing the energy / force to zero over a specified width $\alpha$ leading up to $r_{max}$ $$ r_{trunc} =  r - (r_{max} - \alpha $$ $$ E^{cosine}_{ij} = \begin{cases} E^{SR}_{ij} \left( \frac{1}{2} \left( \cos(\pi \frac{r_{trunc}}{\alpha}) + 1 \right) \right),& r_{trunc} \ge 0 \\\ E^{SR}_{ij},& \text{otherwise} \end{cases} $$ $$ F^{cosine}_{ij} = \begin{cases} \left(F^{SR}_{ij} \left( \frac{1}{2} \left( \cos(\pi \frac{r_{trunc}}{\alpha}) + 1 \right) \right) \right) \left( -E^{SR}_{ij} \pi \sin(\frac{\pi r_{trunc}}{\alpha})\frac{1}{\alpha} \right),& r_{trunc} \ge 0 \\\ F^{SR}_{ij},& \text{otherwise} \end{cases} $$ |
|`Shifted`|--|The short-range interaction is shifted such that, at $r = r_{max}$, the contribution to the energy and force is zero. Internally, the energy and force of the interaction at the limit $r_{max}$ are pre-calculated $$ E^{shifted}_{ij} = E^{SR}_{ij} - \left(r - r_{max}\right) F^{r_{max}}_{ij} - E^{r_{max}}_{ij} $$ $$ F^{shifted}_{q_iq_j} = F^{SR}_{ij} - F^{r_{max}}_{ij} $$ |
