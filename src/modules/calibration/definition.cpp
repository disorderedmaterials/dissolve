// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/calibration/calibration.h"

// Return type of module
std::string_view CalibrationModule::type() const { return "Calibrate"; }

// Return category for module
std::string_view CalibrationModule::category() const { return "Optimisation"; }

// Return brief description of module
std::string_view CalibrationModule::brief() const { return "Calibrate various parameters"; }

// Return the number of Configuration targets this Module requires
int CalibrationModule::nRequiredTargets() const { return Module::ZeroTargets; }
