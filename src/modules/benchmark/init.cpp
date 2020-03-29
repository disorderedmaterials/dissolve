/*
	*** Benchmark Module - Initialisation
	*** src/modules/benchmark/init.cpp
	Copyright T. Youngs 2012-2020

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

#include "modules/benchmark/benchmark.h"
#include "keywords/types.h"

// Perform any necessary initialisation for the Module
void BenchmarkModule::initialise()
{
	keywords_.add("Calculation", new IntegerKeyword(5, 1, 1000), "N", "Number of times to run each benchmark in order to form average");
	keywords_.add("Calculation", new BoolKeyword(true), "Save", "Whether to save new timings to the restart file");
	keywords_.add("Tests", new BoolKeyword(true), "TestIntraEnergy", "Whether to benchmark intramolecular energy calculation");
	keywords_.add("Tests", new BoolKeyword(true), "TestInterEnergy", "Whether to benchmark interatomic energy calculation");
	keywords_.add("Tests", new BoolKeyword(true), "TestGenerator", "Whether to benchmark configuration generator");
	keywords_.add("Tests", new BoolKeyword(true), "TestRDFCells", "Whether to benchmark the RDF cells method (to half-cell range)");
	keywords_.add("Tests", new BoolKeyword(false), "TestRDFSimple", "Whether to benchmark the RDF simple method (to half-cell range)");
	keywords_.add("Tests", new BoolKeyword(true), "TestDistributors", "Whether to benchmark molecule distributors");
}
