// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/configuration.h"
#include "modules/md/md.h"

// Cap forces in Configuration
int MDModule::capForces(double maxForce, std::vector<Vec3<double>> &f)
{
    double fMag;
    const auto maxForceSq = maxForce * maxForce;
    auto nCapped = 0;
    for (auto &fxyz : f)
    {
        fMag = fxyz.magnitudeSq();
        if (fMag < maxForceSq)
            continue;

        fMag = maxForce / sqrt(fMag);
        fxyz *= fMag;

        ++nCapped;
    }

    return nCapped;
}

// Determine timestep based on maximal force component
double MDModule::determineTimeStep(const std::vector<Vec3<double>> &f)
{
    auto fMax = 0.0;
    for (auto &fxyz : f)
    {
        auto m = fxyz.absMax();
        if (m > fMax)
            fMax = m;
    }

    return 1.0 / fMax;
}
