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

	// Print summary
	if (!targetModule) Messenger::print("DataTest: No target Module specified for data location - only tags will be searched.\n");
	else Messenger::print("DataTest: Target Module '%s' will be used as search prefix for data.\n", targetModule->uniqueName());
	Messenger::print("DataTest: Threshold is %f%%.", testThreshold);
	Messenger::print("\n");

	// Loop over reference one-dimensional data supplied
	ListIterator<Data1D> data1DIterator(testData_.data());
	while (Data1D* testData = data1DIterator.iterate())
	{
		bool found = false;

		if (targetModule)
		{
			// Get target module data list
			GenericList& moduleData = targetModule->configurationLocal() ? targetModule->targetConfigurations().firstItem()->moduleData() : dissolve.processingModuleData();

			// Does the named data (with module prefix) exist in the target list?
			if (moduleData.contains(testData->name(), targetModule->uniqueName()))
			{
				// The named data exists - is it of the correct type?
				if (!moduleData.isItemOfType(Data1D::itemClassName(), testData->name(), targetModule->uniqueName())) return Messenger::error("Data named '%s_%s' exists, but is not of the correct type (is %s rather than Data1D).\n", targetModule->uniqueName(), testData->name(), moduleData.find(testData->name(), targetModule->uniqueName())->itemClassName());

				// All is OK, so get the data and check the error against the test set
				Data1D& data = GenericListHelper<Data1D>::retrieve(moduleData, testData->name(), targetModule->uniqueName());
				double error = Error::percent(data, *testData, true);
				Messenger::print("Reference data '%s' (module '%s') has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", testData->name(), targetModule->uniqueName(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
				if (error > testThreshold) return false;
				found = true;
			}
			else if (moduleData.contains(testData->name()))
			{
				// The named data exists without the module name prefix - is it of the correct type?
				if (!moduleData.isItemOfType(Data1D::itemClassName(), testData->name())) return Messenger::error("Data named '%s' exists, but is not of the correct type (is %s rather than Data1D).\n", testData->name(), moduleData.find(testData->name())->itemClassName());

				// All is OK, so get the data and check the error against the test set
				Data1D& data = GenericListHelper<Data1D>::retrieve(moduleData, testData->name());
				double error = Error::percent(data, *testData, true);
				Messenger::print("Reference data '%s' has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", testData->name(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
				if (error > testThreshold) return false;
				found = true;
			}
		}

		// If we haven't found it yet, try a search by object tag
		if ((!found) && Data1D::findObject(testData->name()))
		{
			// The tagged data exists...
			Data1D* data = Data1D::findObject(testData->name());
			double error = Error::percent(*data, *testData, true);
			Messenger::print("Reference data '%s' has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", testData->name(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
			if (error > testThreshold) return false;
			found = true;
		}

		// Did we succeed?
		if (!found)
		{
			if (targetModule) return Messenger::error("No data named '%s_%s' or '%s', or tagged '%s', exists.\n", targetModule->uniqueName(), testData->name(), testData->name(), testData->name());
			else return Messenger::error("No data with tag '%s' exists.\n", testData->name());
		}
	}

	return true;
}

