/*
	*** Calculate Distance-Angle Module - Options
	*** src/modules/calculate/dangle/options.cpp
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

#include "modules/calculate/dangle/dangle.h"
#include "keywords/types.h"

// Set up keywords for Module
void CalculateDAngleModule::setUpKeywords()
{
	// Quantity Name
	KeywordGroup* group = addKeywordGroup("Quantity Name");
	group->add(new CharStringKeyword(), "Name", "Set the name of the calculated distance-angle map", "<name>");

	// Calculation
	group = addKeywordGroup("Calculation");
	group->add(new DoubleKeyword(1.0, 0.0001, 90.0), "AngleBin", "Width of angle bins", "<dtheta, degrees>");
	group->add(new DoubleKeyword(180.0, 0.0, 180.0), "AngleMax", "Maximum angle to bin", "<theta, degrees>");
	group->add(new DoubleKeyword(0.0, 0.0, 180.0), "AngleMin", "Minimum angle to bin", "<theta, degrees>");
	group->add(new DoubleKeyword(0.05, 0.0001), "DistanceBin", "Width of distance bins", "<dr, Angstroms>");
	group->add(new DoubleKeyword(10.0, 0.0), "DistanceMax", "Maximum distance to bin", "<r, Angstroms>");
	group->add(new DoubleKeyword(0.0, 0.0), "DistanceMin", "Minimum distance to bin", "<r, Angstroms>");

	// Sites
	group = addKeywordGroup("Sites");
	group->add(new SpeciesSiteReferenceListKeyword(aSites_), "SiteA", "Add a site which represents 'A' in the interaction A-B...C", "<Species> <Site> [<Species> <Site> ... ]");
	group->add(new SpeciesSiteReferenceListKeyword(bSites_), "SiteB", "Add a site which represents 'B' in the interaction A-B...C", "<Species> <Site> [<Species> <Site> ... ]");
	group->add(new SpeciesSiteReferenceListKeyword(cSites_), "SiteC", "Add a site which represents 'C' in the interaction A-B...C", "<Species> <Site> [<Species> <Site> ... ]");
	group->add(new BoolKeyword(false), "ExcludeSameMolecule", "Whether to exclude correlations between sites on the same molecule", "<True|False>");

	// Export
	group = addKeywordGroup("Export");
	group->add(new BoolKeyword(false), "Save", "Save calculated data to disk", "<True|False>");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int CalculateDAngleModule::parseComplexKeyword(KeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix)
{
	return -1;
}
