// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/drivenMD/drivenMD.h"
#include "keywords/bool.h"
#include "keywords/double.h"
#include "keywords/moduleVector.h"
#include "keywords/optionalDouble.h"
#include "keywords/optionalInt.h"
#include "keywords/rangeVector.h"
#include "keywords/stdString.h"
#include "keywords/weightedModuleVector.h"

DrivenMDModule::DrivenMDModule() : Module(ModuleTypes::DrivenMD)
{
    keywords_.addTarget<ModuleVectorKeyword>("Target", "Add specified Module (and it's Reference data) as a refinement target",
                                             targets_,
                                             std::vector<ModuleTypes::ModuleType>{ModuleTypes::NeutronSQ, ModuleTypes::XRaySQ});
    keywords_.addTarget<WeightedModuleVectorKeyword>(
        "TargetWeight", "Set relative weighting for specific module targets as they enter into the scattering matrix",
        targetWeights_, std::vector<ModuleTypes::ModuleType>{ModuleTypes::NeutronSQ, ModuleTypes::XRaySQ});

    keywords_.setOrganisation("Options", "Control");
    keywords_.add<OptionalIntegerKeyword>("ModifyPotential",
                                          "Frequency at which to apply generated perturbations to interatomic potentials",
                                          modifyPotential_, 0, std::nullopt, 1, "Off");
    keywords_.setOrganisation(
        "Advanced", "Additional R-Factors",
        "Specify additional Q-ranges over which to calculate R-factors in addition to that over the whole Q-range.");
    keywords_.add<RangeVectorKeyword>("RFactorRanges", "Ranges over which to calculate RFactors", ranges_);
}

// Set whether to apply this module's generated potentials to the global pair potentials
void DrivenMDModule::setApplyPotentials(bool b) { applyPotentials_ = b; }
