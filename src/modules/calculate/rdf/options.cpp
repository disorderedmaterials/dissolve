/*
	*** CalculateRDF Module - Options
	*** src/modules/calculate/rdf/options.cpp
	Copyright T. Youngs 2012-2018

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

#include "modules/calculate/rdf/calculaterdf.h"
#include "module/keywordtypes.h"

// Set up keywords for Module
void CalculateRDFModule::setUpKeywords()
{
	keywords_.add(new DoubleModuleKeyword(0.05, 0.0001), "BinWidth", "Width of bins in histogram (and spacing between points in resulting RDF)", "<r, Angstroms>");
	keywords_.add(new SpeciesSiteModuleKeyword(), "OriginSite", "Set the site which is to represent the origin of the RDF", "<Species> <Site>");
	keywords_.add(new SpeciesSiteModuleKeyword(), "OtherSite", "Set the site for which the distribution around the origin site is to be calculated", "<Species> <Site>");
	keywords_.add(new CharStringModuleKeyword(), "Name", "Set the name of the calculated RDF", "<name>");
	keywords_.add(new DoubleModuleKeyword(10.0, 0.0), "RMax", "Maximum distance to bin in RDF", "<r, Angstroms>");
	keywords_.add(new DoubleModuleKeyword(0.0, 0.0), "RMin", "Minimum distance to bin in RDF", "<r, Angstroms>");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int CalculateRDFModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix)
{
	return -1;
}
