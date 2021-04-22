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
    auto fMax = *std::max_element(f.begin(), f.end(), [](auto &left, auto &right) { return left.absMax() < right.absMax(); });

    return 1.0 / fMax.absMax();
}
