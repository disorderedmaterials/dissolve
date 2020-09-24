// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/sq/sq.h"

// Perform any necessary initialisation for the Module
void SQModule::initialise()
{
    // Q range
    keywords_.add("Calculation", new DoubleKeyword(0.05, 1.0e-5), "QDelta", "Step size in Q for S(Q) calculation");
    keywords_.add("Calculation", new DoubleKeyword(-1.0, -1.0), "QMax", "Maximum Q for calculated S(Q)");
    keywords_.add("Calculation", new DoubleKeyword(0.01, 0.0), "QMin", "Minimum Q for calculated S(Q)");
    keywords_.add("Calculation", new BroadeningFunctionKeyword(BroadeningFunction()), "QBroadening",
                  "Instrument broadening function to apply when calculating S(Q)");
    keywords_.add("Calculation", new WindowFunctionKeyword(WindowFunction(WindowFunction::NoWindow)), "WindowFunction",
                  "Window function to apply in Fourier-transform of g(r) to S(Q)");

    // Export
    keywords_.add("Export", new BoolKeyword(false), "Save", "Whether to save partials to disk after calculation",
                  "<True|False>");
}
