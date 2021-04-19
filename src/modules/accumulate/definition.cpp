// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/accumulate/accumulate.h"

// Return type of module
std::string_view AccumulateModule::type() const { return "Accumulate"; }

// Return category for module
std::string_view AccumulateModule::category() const { return "NO CATEGORY ASSIGNED"; }

// Return brief description of module
std::string_view AccumulateModule::brief() const { return "Accumulate partials data to form an average"; }

// Return the number of Configuration targets this Module requires
int AccumulateModule::nRequiredTargets() const { return Module::ZeroTargets; }
