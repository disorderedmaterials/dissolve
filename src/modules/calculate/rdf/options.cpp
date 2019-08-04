/*
	*** Calculate RDF Module - Options
	*** src/modules/calculate/rdf/options.cpp
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

#include "modules/calculate/rdf/rdf.h"
#include "keywords/types.h"

// Set up keywords for Module
void CalculateRDFModule::setUpKeywords()
{
	// Quantity Name
	ModuleKeywordGroup* group = addKeywordGroup("Quantity Name");
	group->add(new CharStringModuleKeyword(), "Name", "Set the name of the calculated RDF", "<name>");

	// Calculation
	group = addKeywordGroup("Calculation");
	group->add(new DoubleModuleKeyword(0.05, 0.0001), "BinWidth", "Width of bins in histogram (and spacing between points in resulting RDF)", "<r, Angstroms>");
	group->add(new DoubleModuleKeyword(10.0, 0.0), "RMax", "Maximum distance to bin in RDF", "<r, Angstroms>");
	group->add(new DoubleModuleKeyword(0.0, 0.0), "RMin", "Minimum distance to bin in RDF", "<r, Angstroms>");

	// Sites
	group = addKeywordGroup("Sites");
	group->add(new SpeciesSiteReferenceListModuleKeyword(originSites_), "OriginSite", "Set the site(s) which are to represent the origin of the RDF", "<Species> <Site>");
	group->add(new SpeciesSiteReferenceListModuleKeyword(otherSites_), "OtherSite", "Set the site(s) for which the distribution around the origin site should be calculated", "<Species> <Site>");
	group->add(new BoolModuleKeyword(false), "ExcludeSameMolecule", "Whether to exclude correlations between sites on the same molecule", "<True|False>");

	// Export
	group = addKeywordGroup("Export");
	group->add(new BoolModuleKeyword(false), "Save", "Save calculated data to disk", "<True|False>");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int CalculateRDFModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix)
{
	return -1;
}
