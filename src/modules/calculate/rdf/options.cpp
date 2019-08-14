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
	keywords_.add("Quantity Name", new CharStringKeyword(), "Name", "Set the name of the calculated RDF", "<name>");

	// Calculation
	keywords_.add("Calculation", new DoubleKeyword(0.05, 0.0001), "BinWidth", "Width of bins in histogram (and spacing between points in resulting RDF)", "<r, Angstroms>");
	keywords_.add("Calculation", new DoubleKeyword(10.0, 0.0), "RMax", "Maximum distance to bin in RDF", "<r, Angstroms>");
	keywords_.add("Calculation", new DoubleKeyword(0.0, 0.0), "RMin", "Minimum distance to bin in RDF", "<r, Angstroms>");

	// Sites
	keywords_.add("Sites", new SpeciesSiteReferenceListKeyword(originSites_), "OriginSite", "Set the site(s) which are to represent the origin of the RDF", "<Species> <Site>");
	keywords_.add("Sites", new SpeciesSiteReferenceListKeyword(otherSites_), "OtherSite", "Set the site(s) for which the distribution around the origin site should be calculated", "<Species> <Site>");
	keywords_.add("Sites", new BoolKeyword(false), "ExcludeSameMolecule", "Whether to exclude correlations between sites on the same molecule", "<True|False>");

	// Export
	keywords_.add("Export", new BoolKeyword(false), "Save", "Save calculated data to disk", "<True|False>");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int CalculateRDFModule::parseComplexKeyword(KeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix)
{
	return -1;
}
