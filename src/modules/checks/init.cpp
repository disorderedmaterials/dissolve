// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/checks/checks.h"

// Perform any necessary initialisation for the Module
void ChecksModule::initialise()
{
    // Targets
    keywords_.addTarget<ConfigurationVectorKeyword>("Configuration", "Set target configuration(s) for the module",
                                                    targetConfigurations_);

    // Distance
    keywords_.add<GeometryListKeyword>("Distance", "Distance", "Define a distance between Atoms to be checked", distances_,
                                       Geometry::DistanceType);
    keywords_.add<DoubleKeyword>("Distance", "DistanceThreshold", "Threshold at which distance checks will fail (Angstroms)",
                                 distanceThreshold_, 1.0e-5);

    // Angle
    keywords_.add<GeometryListKeyword>("Angle", "Angle", "Define an angle between Atoms to be checked", angles_,
                                       Geometry::AngleType);
    keywords_.add<DoubleKeyword>("Angle", "AngleThreshold", "Threshold at which angle checks will fail", angleThreshold_,
                                 1.0e-5);
}
