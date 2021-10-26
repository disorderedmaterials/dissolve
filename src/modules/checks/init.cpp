// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/checks/checks.h"

// Perform any necessary initialisation for the Module
void ChecksModule::initialise()
{
    // Distance
    keywords_.add("Distance", new GeometryListKeyword({}, Geometry::DistanceType), "Distance",
                  "Define a distance between Atoms to be checked");
    keywords_.add<DoubleKeyword>("Distance", "DistanceThreshold", "Threshold at which distance checks will fail (Angstroms)",
                                 distanceThreshold_, 1.0e-5);

    // Angle
    keywords_.add("Angle", new GeometryListKeyword({}, Geometry::AngleType), "Angle",
                  "Define an angle between Atoms to be checked");
    keywords_.add<DoubleKeyword>("Angle", "AngleThreshold", "Threshold at which angle checks will fail", angleThreshold_,
                                 1.0e-5);
}
