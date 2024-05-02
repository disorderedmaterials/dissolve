// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "math/function1D.h"
#include "math/windowFunction.h"

// Forward Declarations
class Data1D;

// Fourier Transforms
namespace Fourier
{
// Perform Fourier sine transform of supplied data, over range specified, and with specified window and broadening
// functions applied
bool sineFT(Data1D &data, double normFactor, double wMin, double wStep, double wMax,
            WindowFunction windowFunction = WindowFunction(),
            const Functions::Function1DWrapper &broadening = Functions::Function1DWrapper());
}; // namespace Fourier
