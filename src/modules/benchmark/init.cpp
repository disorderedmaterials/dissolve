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
}
