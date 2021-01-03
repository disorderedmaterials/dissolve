// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/checks/checks.h"

// Perform any necessary initialisation for the Module
void ChecksModule::initialise()
{
    // Distance
    keywords_.add("Distance", new GeometryListKeyword(distances_, Geometry::DistanceType), "Distance",
                  "Define a distance between Atoms to be checked", "<i> <j> <referenceDistance>");
    keywords_.add("Distance", new DoubleKeyword(0.001, 1.0e-5), "DistanceThreshold",
                  "Threshold at which distance checks will fail (Angstroms)", "<threshold[0.001]>");

    // Angle
    keywords_.add("Angle", new GeometryListKeyword(angles_, Geometry::AngleType), "Angle",
                  "Define an angle between Atoms to be checked", "<i> <j> <k> <referenceAngle>");
    keywords_.add("Angle", new DoubleKeyword(0.05, 1.0e-5), "AngleThreshold", "Threshold at which angle checks will fail",
                  "<threshold[0.05]>");
}
