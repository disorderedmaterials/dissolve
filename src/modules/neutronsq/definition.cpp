// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/neutronsq/neutronsq.h"

// Return type of module
std::string_view NeutronSQModule::type() const { return "NeutronSQ"; }

// Return category for module
std::string_view NeutronSQModule::category() const { return "Correlation Functions"; }

// Return brief description of module
std::string_view NeutronSQModule::brief() const { return "Calculate neutron-weighted S(Q)"; }

// Return the number of Configuration targets this Module requires
int NeutronSQModule::nRequiredTargets() const { return Module::ZeroTargets; }
