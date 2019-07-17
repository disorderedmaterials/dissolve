/*
	*** DataTest Module - Processing
	*** src/modules/datatest/process.cpp
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

#include "modules/datatest/datatest.h"
#include "main/dissolve.h"
#include "math/error.h"
#include "base/sysfunc.h"
#include "genericitems/listhelper.h"

// Run main processing
bool DataTestModule::process(Dissolve& dissolve, ProcessPool& procPool)
{
	/*
	 * This is a serial routine.
	 */

	// Get options and target Module
	const double testThreshold = keywords_.asDouble("Threshold");
	Module* targetModule = targetModule_.firstItem();
	Error::ErrorType errorType = KeywordEnumHelper<Error::ErrorType>::enumeration(keywords_, "ErrorType");

	// Print summary
	if (!targetModule) Messenger::print("DataTest: No target Module specified for data location - only tags will be searched.\n");
	else Messenger::print("DataTest: Target Module '%s' will be used as search prefix for data.\n", targetModule->uniqueName());
	Messenger::print("DataTest: Error calculation is '%s', threshold is %e.", Error::errorTypes().keyword(errorType), testThreshold);
	Messenger::print("\n");

	// Loop over reference one-dimensional data supplied
	ListIterator<Data1D> data1DIterator(test1DData_.data());
	while (Data1D* testData = data1DIterator.iterate())
	{
		// Locate the target reference data
		bool found = false;
		const Data1D& data = findReferenceData<Data1D>(testData->name(), targetModule, dissolve.processingModuleData(), found);

		// Did we succeed?
		if (!found)
		{
			if (targetModule) return Messenger::error("No data named '%s_%s' or '%s', or tagged '%s', exists.\n", targetModule->uniqueName(), testData->name(), testData->name(), testData->name());
			else return Messenger::error("No data with tag '%s' exists.\n", testData->name());
		}
		Messenger::print("Located reference data with tag '%s'.\n", data.objectTag());

		// Generate the error estimate and compare against the threshold value
		double error = Error::error(errorType, data, *testData, true);
		Messenger::print("Target data '%s' has error of %7.3f with calculated data and is %s (threshold is %6.3e)\n\n", testData->name(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
		if (error > testThreshold) return false;
	}

	return true;
}

