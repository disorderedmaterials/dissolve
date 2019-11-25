/*
	*** Bragg Module - Initialisation
	*** src/modules/bragg/init.cpp
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

#include "modules/bragg/bragg.h"
#include "keywords/types.h"
#include "math/averaging.h"

// Perform any necessary initialisation for the Module
void BraggModule::initialise()
{
	// Calculation
	keywords_.add("Calculation", new IntegerKeyword(5, 1), "Averaging", "Number of historical data sets to combine into final reflection data", "<5>");
	keywords_.add("Calculation", new EnumOptionsKeyword<Averaging::AveragingScheme>(Averaging::averagingSchemes() = Averaging::LinearAveraging), "AveragingScheme", "Weighting scheme to use when averaging reflection data", "<Linear>");
	keywords_.add("Calculation", new DoubleKeyword(0.001), "QDelta", "Resolution (binwidth) in Q space to use when calculating Bragg reflections", "<0.001>");
	keywords_.add("Calculation", new DoubleKeyword(1.0), "QMax", "Maximum Q value for Bragg calculation", "<1.0>");
	keywords_.add("Calculation", new DoubleKeyword(0.01), "QMin", "Minimum Q value for Bragg calculation", "<0.01>");
	keywords_.add("Calculation", new Vec3IntegerKeyword(Vec3<int>(1,1,1), Vec3<int>(1,1,1), Vec3Labels::MinMaxDeltaLabels), "Multiplicity", "Bragg intensity scaling factor accounting for number of repeat units in Configuration", "<1 1 1>");

	// Export
	keywords_.add("Export", new BoolKeyword(false), "SavePartials", "Whether to save Bragg partials to disk after calculation", "<True|False>");
	keywords_.add("Export", new BoolKeyword(false), "SaveReflections", "Whether to save Bragg reflection data to disk", "<True|False>");
}
