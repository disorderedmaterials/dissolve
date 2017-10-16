/*
	*** Test Module - Method
	*** src/modules/test/method.cpp
	Copyright T. Youngs 2012-2017

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "modules/test/test.h"
#include "main/duq.h"
#include "classes/scatteringmatrix.h"
#include "classes/weights.h"
#include "classes/atomtype.h"
#include "base/sysfunc.h"

// Perform set up tasks for module
bool TestModule::setup(ProcessPool& procPool)
{
	return true;
}

// Execute pre-processing stage
bool TestModule::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Execute Method
bool TestModule::process(DUQ& duq, ProcessPool& procPool)
{
	// First, make sure that all of the associated Data is setup
	RefListIterator<Data,bool> dataIterator(targetData_);
	while (Data* data = dataIterator.iterate())
	{
		if (!data->setup(duq.processingModuleData())) return false;
		data->data().save(CharString("%s-setup.txt", data->name()));
	}

	/*
	 * Construct our full, square, scattering matrix, using the master AtomTypes list
	 */
	ScatteringMatrix scatteringMatrix;
	scatteringMatrix.initialise(duq.atomTypeList());

	// For each Data, get the associated Weights from the associated Partials module
	dataIterator.restart();
	while (Data* data = dataIterator.iterate())
	{
		bool found;
		Weights& weights = GenericListHelper<Weights>::retrieve(duq.processingModuleData(), "FullWeights", data->associatedModule()->uniqueName(), Weights(), &found);
		if (!found)
		{
			Messenger::error("TestModule: Couldn't find FullWeights for Data '%s', and so can't construct scattering matrix.\n", data->name());
			return false;
		}

		// Set the next row of the scattering matrix with the weights of the supplied data.
		if (!scatteringMatrix.addReferenceData(data, weights))
		{
			Messenger::error("TestModule: Failed to initialise reference Data.\n");
			return false;
		}
	}
	if (!scatteringMatrix.finalise())
	{
		Messenger::error("TestModule: Failed to set up scattering matrix.\n");
		return false;
	}

	scatteringMatrix.print();

	/*
	 * Use the ScatteringMatrix to generate partials from the supplied reference data
	 */
	scatteringMatrix.generatePartials();

	/*
	 * Construct difference matrix of partials
	 */
	Array2D<XYData> differences;
	for (AtomType* at1 = duq.atomTypeList().first(); at1 != NULL; at1 = at1->next)
	{
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next)
		{
		}
	}

	return true;
}

// Execute post-processing stage
bool TestModule::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}
