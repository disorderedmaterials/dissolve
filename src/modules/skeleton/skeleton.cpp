// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/skeleton/skeleton.h"

SkeletonModule::SkeletonModule() : Module() {}

// Return type of module
std::string_view SkeletonModule::type() const { return "Skeleton"; }
