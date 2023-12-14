// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/bragg/bragg.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/double.h"
#include "keywords/optionalInt.h"
#include "keywords/stdString.h"
#include "keywords/vec3Integer.h"

BraggModule::BraggModule() : Module(ModuleTypes::Bragg)
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation(
        "Options", "Control",
        "Q range over which to calculate Bragg scattering contributions and the effective multiplicity of the unit cell.");
    keywords_.add<DoubleKeyword>("QMin", "Minimum Q value for Bragg calculation", qMin_, 0.0)
        ->setEditSignals({KeywordBase::ClearModuleData, KeywordBase::RecreateRenderables});
    keywords_.add<DoubleKeyword>("QMax", "Maximum Q value for Bragg calculation", qMax_, 0.0)
        ->setEditSignals({KeywordBase::ClearModuleData, KeywordBase::RecreateRenderables});
    keywords_
        .add<DoubleKeyword>("QDelta", "Resolution (bin width) in Q space to use when calculating Bragg reflections", qDelta_,
                            1.0e-5)
        ->setEditSignals({KeywordBase::ClearModuleData, KeywordBase::RecreateRenderables});
    keywords_
        .add<Vec3IntegerKeyword>("Multiplicity",
                                 "Bragg intensity scaling factor accounting for number of repeat units in Configuration",
                                 multiplicity_, Vec3<int>(1, 1, 1), std::nullopt, Vec3Labels::HKLLabels)
        ->setEditSignals({KeywordBase::ClearModuleData, KeywordBase::RecreateRenderables});

    keywords_.setOrganisation("Options", "Averaging");
    keywords_.add<OptionalIntegerKeyword>("Averaging", "Number of historical data sets to combine into final reflection data",
                                          averagingLength_, 1, std::nullopt, 1, "Off");
    keywords_.add<EnumOptionsKeyword<Averaging::AveragingScheme>>("AveragingScheme",
                                                                  "Weighting scheme to use when averaging reflection data",
                                                                  averagingScheme_, Averaging::averagingSchemes());

    keywords_.setOrganisation("Export");
    keywords_.add<BoolKeyword>("SaveReflections", "Whether to save Bragg reflection data to disk", saveReflections_);
}
