// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "modules/bragg/bragg.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/double.h"
#include "keywords/integer.h"
#include "keywords/stdstring.h"
#include "keywords/vec3integer.h"

BraggModule::BraggModule() : Module("Bragg")
{
    // Targets
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    // Control
    keywords_.add<IntegerKeyword>("Control", "Averaging",
                                  "Number of historical data sets to combine into final reflection data", averagingLength_, 0);
    keywords_.add<EnumOptionsKeyword<Averaging::AveragingScheme>>("Control", "AveragingScheme",
                                                                  "Weighting scheme to use when averaging reflection data",
                                                                  averagingScheme_, Averaging::averagingSchemes());
    keywords_.add<DoubleKeyword>(
        "Control", "QDelta", "Resolution (bin width) in Q space to use when calculating Bragg reflections", qDelta_, 1.0e-5);
    keywords_.add<DoubleKeyword>("Control", "QMax", "Maximum Q value for Bragg calculation", qMax_, 0.0);
    keywords_.add<DoubleKeyword>("Control", "QMin", "Minimum Q value for Bragg calculation", qMin_, 0.0);
    keywords_.add<Vec3IntegerKeyword>("Control", "Multiplicity",
                                      "Bragg intensity scaling factor accounting for number of repeat units in Configuration",
                                      multiplicity_, Vec3<int>(1, 1, 1), std::nullopt, Vec3Labels::HKLLabels);

    // Export
    keywords_.add<BoolKeyword>("Export", "SaveReflections", "Whether to save Bragg reflection data to disk", saveReflections_);

    // Test
    keywords_.addKeyword<StringKeyword>(
        "TestReflections", "Whether to test calculated reflection data against that in specified file", testReflectionsFile_);
}
