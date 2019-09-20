/*
	*** Checks Module - Initialisation
	*** src/modules/checks/init.cpp
	Copyright T. Youngs 2012-2019

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "modules/checks/checks.h"
#include "keywords/types.h"
#include "base/lineparser.h"
#include "keywords/geometrylist.h"


// Perform any necessary initialisation for the Module
void ChecksModule::initialise()
{
	// Distance
	keywords_.add("Distance", new GeometryKeyword(distances_, Geometry::GeometryType::Distance), "Distance", "Define a distance between Atoms to be checked", "<i> <j> <referenceDistance>");
	keywords_.add("Distance", new DoubleKeyword(0.001, 1.0e-5), "DistanceThreshold", "Threshold at which distance checks will fail (Angstroms)", "<threshold[0.001]>");

	// Angle
	keywords_.add("Angle", new GeometryKeyword(angles_, Geometry::GeometryType::Distance), "Angle", "Define an angle between Atoms to be checked", "<i> <j> <k> <referenceAngle>");
	keywords_.add("Angle", new DoubleKeyword(0.05, 1.0e-5), "AngleThreshold", "Threshold at which angle checks will fail", "<threshold[0.05]>");
}

