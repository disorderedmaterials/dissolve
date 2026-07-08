// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/mcCommon.h"

namespace MCCommon
{
// Update the given step size
double updateStepSize(double currentStepSize, int nMovesAttempted, int nMovesSucceeded, double targetAcceptanceRate,
                      double stepMin, double stepMax)
{
    auto newStepSize =
        currentStepSize * (nMovesSucceeded == 0 ? 0.8 : (double(nMovesSucceeded) / nMovesAttempted) / targetAcceptanceRate);
    if (newStepSize < stepMin)
        newStepSize = stepMin;
    else if (newStepSize > stepMax)
        newStepSize = stepMax;
    return newStepSize;
}
}; // namespace MCCommon
