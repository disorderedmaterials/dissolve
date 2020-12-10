---
title: Window Functions
description: Available windowing functions for use in Fourier transforms
---
Note: Most of the following functions assume an input range of $0 \leq x \leq 1$.

|Keyword|Parameters|Description|
|:---:|:--------:|-----------|
|`None`|--|No window function|
|`Bartlett`|--|Bartlett (triangular) window $$ f(x) = \frac{\lvert x - 0.5\rvert}{0.5} $$|
|`Hann`|--|von Hann (Hanning) window $$ f(x) = \frac{1}{2}\left(1-\cos(2 \pi x)\right) $$ |
|`Lanczos`|--|Lanczos window $$ f(x) = \frac{\sin\left(\pi (2x-1)\right)}{\pi (2x-1)} $$ |
|`Nuttall`|--|Nuttall window (continuous first derivatives over range) $$ f(x) = 0.355768 - 0.487396 \cos(2 \pi x) + 0.144232 \cos(4 \pi x) - 0.012604 \cos(6 \pi x) $$ |
|`Sine`|--|Sine Window $$ f(x) = 1 - \sin\left(\frac{1}{2} \pi x\right) $$ |
|`Lorch0`|--|Original Lorch function - $x$ here represents the real value of the current position, rather than one converted to the range $0 \leq x \leq 1$. $$ f(x) = \frac{\sin(x \Delta_0)}{x \Delta_0}, \Delta_0 = \frac{pi}{x_{max}} $$ |
