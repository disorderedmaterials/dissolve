---
parent: Developer Documentation
nav_order: 1
mermaid: true
---
# Release Cycle

## Introduction

### Purpose
This document defines the versioning and release cycle intended for the Dissolve project.

### Scope
This document describes and defines the release cycle and versioning for the Dissolve project in more detail, suitable for users, contributors, and developers.

## Overview

Dissolve is a scientific code with a well-defined underlying framework (which is likely to change slowly) upon which specific functionality is added in a [modular manner](overviews/modules.md). As such, the development cycle is likely to be fairly rapid. For these reasons, a three-month release cycle will be undertaken in the first instance, beginning from `version 1.0` planned for January 2020.

```mermaid
gantt
	dateFormat  YYYY-MM-DD
	title Proposed Dissolve release schedule

	section Events
	DM UGM and Workshop:    workshop1, 2020-01-22, 3d
	Neutron Training Course:    workshop2, 2020-03-19, 2d

	section v1.x
	v1.0.0 (Development):     v10, 2019-06-01, 180d
	v1.0.x (Hotfixes) :       crit, v10hf, after v10, 55d
	v1.1.0 (Development):     v11, after workshop1, 80d
	v1.1.x (Hotfixes) :       crit, v11hf, after v11, 10d
	v1.2.0 (Development):     v12, after v11hf, 80d
	v1.2.x (Hotfixes) :       crit, v12hf, after v12, 10d
	v1.3.0 (Development):     v13, after v12hf, 80d
	v1.3.x (Hotfixes) :       crit, v13hf, after v13, 10d
```

Development priorities for each incremental version will be decided by the [project management committee](../governance/pmc.md).
