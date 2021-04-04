---
title: Error Types
description: Error types used when comparing datasets
---

|Keyword|Description|
|:---:|-----------|
|`RMSE`|Root mean squared error: $$\Delta x = \sqrt{\sum_{i=1}^{N} (x_i - x_i^{ref})^2 \frac{1}{N}}$$|
|`MAAPE`|Mean arctangent average percentage error: $$\Delta x = 100 \sum_{i=1}^{N} \arctan{\left\|\frac{x_i - x_i^{ref}}{x_i}\right\|}$$|
|`MAPE`|Mean average percentage error: $$\Delta x = 100 \sum_{i=1}^{N} \left\|\frac{x_i - x_i^{ref}}{x_i}\right\|$$|
|`Percent`|Basic percentage error: $$\Delta x = 100 \frac{\sum_{i=1}^{N} \left\|x_i - x_i^{ref}\right\|}{\sum_{i=1}^{N} \left\|y_i\right\|}$$|
|`RFactor`|EPSR-style r-factor, equivalent to the mean squared error: $$\Delta x = \sum_{i=1}^{N} (x_i - x_i^{ref})^2 \frac{1}{N}$$|
|`Euclidean`|Euclidean distance: $$\Delta x = \frac{\sum_{i=1}^{N} \left\|x_i - x_i^{ref}\right\|}{\sum_{i=1}^{N} \left\|x_i^2\right\|}$$|
