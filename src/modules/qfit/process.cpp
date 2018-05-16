/*
	*** QFit Module - Processing
	*** src/modules/qfit/process.cpp
	Copyright T. Youngs 2012-2018

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

#include "modules/qfit/qfit.h"
#include "main/duq.h"
#include "base/sysfunc.h"
#include "templates/genericlisthelper.h"

// Return whether the Module has a processing stage
bool QFitModule::hasProcessing()
{
	return true;
}

// Run main processing
bool QFitModule::process(DUQ& duq, ProcessPool& procPool)
{
	/*
	 * Grab dependent Module pointers
	 */
	Module* partialsModule = dependentModule("Partials");
	if (!partialsModule)
	{
		Messenger::error("No Partials Module associated to QFitMoodule '%s'.\n", uniqueName());
		return false;
	}

	/*
	 * First, make sure that all of the associated Data are set up
	 */
	if (targetData_.nItems() == 0) return Messenger::error("No reference data provided to fit against.\n");
	RefListIterator<Data,bool> dataIterator(targetData_);
	while (Data* data = dataIterator.iterate()) if (!data->setUp(duq.processingModuleData())) return false;

	/*
	 * Loop over current data and perform our various fitting procedures
	 */
	dataIterator.restart();
	double startTotalError = 0.0;
	Array<double> dataSetErrors;
	while (Data* data = dataIterator.iterate())
	{
		/*
		 * IntraBroadening
		 */
		if (true)
		{
			// Must have an associated PartialsModule from which to get the total S(Q)
			if (!data->associatedModule()) continue;
			if (data->isAssociatedModule("Partials"))
			{
				Messenger::print("Requested fitting of IntraBroadening, but the associated Module for Data '%s' is not a PartialsModule (= %s).\n", data->name(), data->associatedModuleName());
				continue;
			}

			Module* partialsModule = data->associatedModule();

			// Retrieve the UnweightedGR from the PartialsModule
			bool found = false;
// 		/*	PartialSet& unweightedGR = GenericListHelper<PartialSet>::retrieve(duq.processingModuleData(), "UnweightedGR", data->associatedModule()->uniqueName(), PartialSet(), &found);
// 			if (!found)
// 			{
// 				Messenger::warn("Could not locate UnweightedGR for Data '%s'.\n", data->name());
// 				continue;
// 			}*/

			// Retrieve and check IntraBroadening option from the PartialsModule keywords
			BroadeningFunction intraBroadening = KeywordListHelper<BroadeningFunction>::retrieve(partialsModule->keywords(), "IntraBroadening", BroadeningFunction());
			if (intraBroadening.function() == BroadeningFunction::NoFunction)
			{
// 				Messenger::print("No IntraBroadening supplied
			}

			// Calculate current percentage error in calculated vs experimental S(Q)
// 			double startError = data->data().error(calcSQ.total());
// 			startTotalError += startError;
			
		
		
		
		}
	}


	Messenger::print("Average error over all specified datasets was %f%%.\n", startTotalError / targetData_.nItems());

	return true;
}

