// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "math/broadeningfunction.h"
#include "math/windowfunction.h"

// Forward Declarations
class Data1D;

// Fourier Transforms
class Fourier
{
    public:
    // Perform Fourier sine transform of supplied data, over range specified, and with specified window and broadening
    // functions applied
    static bool sineFT(Data1D &data, double normFactor, double wMin, double wStep, double wMax,
                       WindowFunction windowFunction = WindowFunction(), BroadeningFunction broadening = BroadeningFunction());
};
