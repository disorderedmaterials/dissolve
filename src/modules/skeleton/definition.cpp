// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/skeleton/skeleton.h"

// Return type of module
std::string_view SkeletonModule::type() const { return "Skeleton"; }

// Return category for module
std::string_view SkeletonModule::category() const { return "HIDDEN"; }

// Return brief description of module
std::string_view SkeletonModule::brief() const
{
    return "Empty Module providing skeletal structure for new Module development";
}

// Return the number of Configuration targets this Module requires
int SkeletonModule::nRequiredTargets() const { return Module::ZeroTargets; }
