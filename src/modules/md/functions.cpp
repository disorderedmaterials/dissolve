// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "classes/configuration.h"
#include "modules/md/md.h"

// Cap forces in Configuration
int MDModule::capForces(Configuration *cfg, double maxForce, Array<double> &fx, Array<double> &fy, Array<double> &fz)
{
    double fMag;
    const auto maxForceSq = maxForce * maxForce;
    auto nCapped = 0;
    for (auto n = 0; n < cfg->nAtoms(); ++n)
    {
        fMag = fx[n] * fx[n] + fy[n] * fy[n] + fz[n] * fz[n];
        if (fMag < maxForceSq)
            continue;

        fMag = maxForce / sqrt(fMag);
        fx[n] *= fMag;
        fy[n] *= fMag;
        fz[n] *= fMag;

        ++nCapped;
    }

    return nCapped;
}

// Determine timestep based on maximal force component
double MDModule::determineTimeStep(const Array<double> &fx, const Array<double> &fy, const Array<double> &fz)
{
    double fMax = fx.maxAbs();
    double fTemp = fy.maxAbs();
    if (fTemp > fMax)
        fMax = fTemp;
    fTemp = fz.maxAbs();
    if (fTemp > fMax)
        fMax = fTemp;

    return 1.0 / fMax;
}
