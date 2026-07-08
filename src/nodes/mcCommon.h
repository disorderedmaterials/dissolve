// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

namespace MCCommon
{
// Update the given step size
double updateStepSize(double currentStepSize, int nMovesAttempted, int nMovesSucceeded, double targetAcceptanceRate,
                      double stepMin, double stepMax);
}; // namespace MCCommon
