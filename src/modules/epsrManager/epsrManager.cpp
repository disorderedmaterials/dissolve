// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/epsrManager/epsrManager.h"
#include "keywords/moduleVector.h"
#include "keywords/optionalInt.h"
#include "keywords/stdString.h"

EPSRManagerModule::EPSRManagerModule() : Module(ModuleTypes::EPSRManager)
{
    keywords_.addTarget<ModuleVectorKeyword>("Target", "Target EPSR modules over which the manager will operate", target_,
                                             std::vector<ModuleTypes::ModuleType>{ModuleTypes::EPSR});

    keywords_.setOrganisation("Options");
    keywords_.add<OptionalIntegerKeyword>("ModifyPotential",
                                          "Frequency at which to apply generated perturbations to interatomic potentials",
                                          modifyPotential_, 0, std::nullopt, 1, "Off");

    // Specific Potential Adjustment
    keywords_.setOrganisation("Options", "Potential Scaling");
    keywords_.add<StringKeyword>("PotScale", "Comma-separated list of atom type pairs and scaling factors in the form A-B=1.0",
                                 potentialScalings_);
    keywords_.setOrganisation("Options", "Averaging");
    keywords_.add<OptionalIntegerKeyword>("Averaging", "Number of historical potential sets to combine into final potentials",
                                          averagingLength_, 1, std::nullopt, 1, "Off");
    keywords_.add<EnumOptionsKeyword<Averaging::AveragingScheme>>("AveragingScheme",
                                                                  "Weighting scheme to use when averaging potentials",
                                                                  averagingScheme_, Averaging::averagingSchemes());
}
