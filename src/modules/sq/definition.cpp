// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/sq/sq.h"

// Return type of module
std::string_view SQModule::type() const { return "SQ"; }

// Return category for module
std::string_view SQModule::category() const { return "Correlation Functions"; }

// Return brief description of module
std::string_view SQModule::brief() const { return "Transform g(r) into unweighted S(Q)"; }

// Return the number of Configuration targets this Module requires
int SQModule::nRequiredTargets() const { return Module::ZeroTargets; }
