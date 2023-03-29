// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/checks/checks.h"
#include "keywords/configuration.h"
#include "keywords/double.h"
#include "keywords/geometrylist.h"

ChecksModule::ChecksModule() : Module(ModuleTypes::Checks)
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Distance");
    keywords_.add<GeometryListKeyword>("Distance", "Define a distance between Atoms to be checked", distances_,
                                       Geometry::DistanceType);
    keywords_.add<DoubleKeyword>("DistanceThreshold", "Threshold at which distance checks will fail (Angstroms)",
                                 distanceThreshold_, 1.0e-5);

    keywords_.setOrganisation("Options", "Angle");
    keywords_.add<GeometryListKeyword>("Angle", "Define an angle between Atoms to be checked", angles_, Geometry::AngleType);
    keywords_.add<DoubleKeyword>("AngleThreshold", "Threshold at which angle checks will fail", angleThreshold_, 1.0e-5);
}
