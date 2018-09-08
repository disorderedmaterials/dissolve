/*
	*** DataTest Module - Processing
	*** src/modules/datatest/process.cpp
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

#include "modules/datatest/datatest.h"
#include "main/dissolve.h"
#include "math/error.h"
#include "base/sysfunc.h"
#include "templates/genericlisthelper.h"

// Run main processing
bool DataTestModule::process(Dissolve& dissolve, ProcessPool& procPool)
{
	/*
	 * This is a serial routine.
	 */

	// Get option values
	const double testThreshold = keywords_.asDouble("TestThreshold");

	// Get target Module
	Module* targetModule = targetModule_.firstItem();
	if (!targetModule) return Messenger::error("No target Module specified for data location.\n");

	// Loop over reference one-dimensional data supplied
	ListIterator<Data1D> data1DIterator(testData_.data1D());
	while (Data1D* testData = data1DIterator.iterate())
	{
		// Does the named data exist in the processing module data?
		if (!dissolve.processingModuleData().contains(testData->name(), targetModule->uniqueName())) return Messenger::error("No data named '%s_%s' exists.\n", targetModule->uniqueName(), testData->name());

		// The named data exists - is it of the correct type?
		if (!dissolve.processingModuleData().isItemOfType(Data1D::itemClassName(), testData->name(), targetModule->uniqueName())) return Messenger::error("Data named '%s_%s' exists, but is not of the correct type (is %s rather than Data1D).\n", targetModule->uniqueName(), testData->name(), dissolve.processingModuleData().find(testData->name(), targetModule->uniqueName())->itemClassName());

		// All is OK, so get the data and check the error against the test set
		Data1D& data = GenericListHelper<Data1D>::retrieve(dissolve.processingModuleData(), testData->name(), targetModule->uniqueName());
		double error = Error::percent(data, *testData);
		Messenger::print("Reference data '%s_%s' has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", testData->name(), targetModule->uniqueName(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
		if (error > testThreshold) return false;
	}

	return true;
}

