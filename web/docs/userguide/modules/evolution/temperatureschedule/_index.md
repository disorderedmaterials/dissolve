---
title: TemperatureSchedule (Module)
linkTitle: TemperatureSchedule
description: Control the temperature of a configuration
---

## Overview

The `TemperatureSchedule` module gives the ability to control the temperature of a configuration, adjusting its towards some target temperature at a predefined maximum rate per iteration of the module.

### Targets

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Configuration`|`Configuration`|--|{{< required-label >}}Target configuration on which to operate.|

### Schedule

|Keyword|Arguments|Default|Description|
|:------|:--:|:-----:|-----------|
|`Rate`|`double`|`1.0`|Maximal absolute change to add or subtract from the configuration's current temperature each time the module runs in order to move it towards the target temperature. If the difference between the target and current temperatures is less than the defined rate, the configuration temperature is set to the target temperature.|
|`TargetTemperature`|`double`|`300.0`|Target temperature for the configuration.|

