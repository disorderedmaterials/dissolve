// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "math/averaging.h"
#include "modules/bragg/bragg.h"

// Perform any necessary initialisation for the Module
void BraggModule::initialise()
{
    // Control
    keywords_.add<IntegerKeyword>("Control", "Averaging",
                                  "Number of historical data sets to combine into final reflection data", averagingLength_, 0);
    keywords_.add(
        "Control",
        new EnumOptionsKeyword<Averaging::AveragingScheme>(Averaging::averagingSchemes() = Averaging::LinearAveraging),
        "AveragingScheme", "Weighting scheme to use when averaging reflection data");
    keywords_.add<DoubleKeyword>(
        "Control", "QDelta", "Resolution (bin width) in Q space to use when calculating Bragg reflections", qDelta_, 1.0e-5);
    keywords_.add<DoubleKeyword>("Control", "QMax", "Maximum Q value for Bragg calculation", qMax_, 0.0);
    keywords_.add<DoubleKeyword>("Control", "QMin", "Minimum Q value for Bragg calculation", qMin_, 0.0);
    keywords_.add("Control", new Vec3IntegerKeyword(Vec3<int>(1, 1, 1), Vec3<int>(1, 1, 1), Vec3Labels::HKLLabels),
                  "Multiplicity", "Bragg intensity scaling factor accounting for number of repeat units in Configuration");

    // Export
    keywords_.add<BoolKeyword>("Export", "SaveReflections", "Whether to save Bragg reflection data to disk", saveReflections_);

    // Test
    keywords_.add("HIDDEN", new StringKeyword(), "TestReflections",
                  "Whether to test calculated reflection data against that in specified file");
}
