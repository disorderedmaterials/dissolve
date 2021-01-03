// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "math/averaging.h"
#include "modules/bragg/bragg.h"

// Perform any necessary initialisation for the Module
void BraggModule::initialise()
{
    // Control
    keywords_.add("Control", new IntegerKeyword(5, 1), "Averaging",
                  "Number of historical data sets to combine into final reflection data", "<5>");
    keywords_.add(
        "Control",
        new EnumOptionsKeyword<Averaging::AveragingScheme>(Averaging::averagingSchemes() = Averaging::LinearAveraging),
        "AveragingScheme", "Weighting scheme to use when averaging reflection data", "<Linear>");
    keywords_.add("Control", new DoubleKeyword(0.001), "QDelta",
                  "Resolution (binwidth) in Q space to use when calculating Bragg reflections", "<0.001>");
    keywords_.add("Control", new DoubleKeyword(1.0), "QMax", "Maximum Q value for Bragg calculation", "<1.0>");
    keywords_.add("Control", new DoubleKeyword(0.01), "QMin", "Minimum Q value for Bragg calculation", "<0.01>");
    keywords_.add("Control", new Vec3IntegerKeyword(Vec3<int>(1, 1, 1), Vec3<int>(1, 1, 1), Vec3Labels::MinMaxDeltaLabels),
                  "Multiplicity", "Bragg intensity scaling factor accounting for number of repeat units in Configuration",
                  "<1 1 1>");

    // Export
    keywords_.add("Export", new BoolKeyword(false), "SavePartials", "Whether to save Bragg partials to disk after calculation",
                  "<True|False>");
    keywords_.add("Export", new BoolKeyword(false), "SaveReflections", "Whether to save Bragg reflection data to disk",
                  "<True|False>");
}
