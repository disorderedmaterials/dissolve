// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/xraysq/xraysq.h"

// Return type of module
std::string_view XRaySQModule::type() const { return "XRaySQ"; }

// Return category for module
std::string_view XRaySQModule::category() const { return "Correlation Functions"; }

// Return brief description of module
std::string_view XRaySQModule::brief() const { return "Calculate x-ray-weighted S(Q)"; }

// Return the number of Configuration targets this Module requires
int XRaySQModule::nRequiredTargets() const { return Module::ZeroTargets; }
