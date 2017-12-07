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
#include "templates/genericlisthelper.h"

// Perform set up tasks for module
bool TestModule::setUp(ProcessPool& procPool)
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
	/*
	 * Grab dependent Module pointers
	 */
	Module* partialsModule = dependentModule("Partials");
	if (!partialsModule)
	{
		Messenger::error("No Partials Module associated to TestModule '%s'.\n", uniqueName());
		return false;
	}

	/*
	 * Set Module data target
	 */
	GenericList& moduleData = configurationLocal_ ? targetConfigurations_.firstItem()->moduleData() : duq.processingModuleData();

	/*
	 * Are the energies of all involved Configurations stable (if OnlyWhenStable option is on)
	 */
	if (keywords_.asBool("OnlyWhenStable"))
	{
		bool anyFailed = false;
		RefListIterator<Configuration,bool> configIterator(partialsModule->targetConfigurations());
		while (Configuration* cfg = configIterator.iterate())
		{
			// First, check that the EnergyStable module data exists
			if (cfg->moduleData().contains("EnergyStable"))
			{
				
				bool stable = GenericListHelper<bool>::retrieve(cfg->moduleData(), "EnergyStable", "");
				if (!stable)
				{
					Messenger::print("TestModule: Energy for Configuration '%s' is not yet stable. No potential refinement will be performed this iteration.\n", cfg->name());
					anyFailed = true;
				}
			}
			else
			{
				Messenger::error("No energy stability information found in Configuration '%s' - check your setup.\n", cfg->name());
				return false;
			}
		}
		if (anyFailed) return true;
	}

	/*
	 * Get calculated S(Q), broadening function (which we will 'invert' for the backtransform) and density of the source Configuration(s)
	 */
	bool found;
	PartialSet& unweightedSQ = GenericListHelper<PartialSet>::retrieve(duq.processingModuleData(), "UnweightedSQ", partialsModule->uniqueName(), PartialSet(), &found);
	if (!found)
	{
		Messenger::error("Couldn't locate 'UnweightedSQ' data in PartialsModule '%s'.\n", partialsModule->uniqueName());
		return false;
	}

	BroadeningFunction broadening = GenericListHelper<BroadeningFunction>::retrieve(duq.processingModuleData(), "QBroadening", partialsModule->uniqueName(), BroadeningFunction::unity(), &found);
	if (!found) Messenger::print("TestModule: No 'QBroadening' specified in PartialsModule '%s', so no un-broadening will be performed.\n", partialsModule->uniqueName());
	broadening.setInverted(true);

	double rho = GenericListHelper<double>::retrieve(duq.processingModuleData(), "Density", partialsModule->uniqueName(), 0.0, &found);
	if (!found)
	{
		Messenger::error("Couldn't locate 'Density' data in PartialsModule '%s'.\n", partialsModule->uniqueName());
		return false;
	}

	/*
	 * First, make sure that all of the associated Data is set up
	 */
	RefListIterator<Data,bool> dataIterator(targetData_);
	while (Data* data = dataIterator.iterate())
	{
		if (!data->setUp(duq.processingModuleData())) return false;
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
	int nTypes = duq.atomTypeList().nItems();
	Array2D<XYData> differences(nTypes, nTypes, true);
	int n = 0;
	for (AtomType* at1 = duq.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++n)
	{
		int m = n;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++m)
		{
			// Grab difference partial
			XYData& partial = differences.ref(n, m);

			// Copy our partial S(Q) generated from the experimental datasets
			partial = scatteringMatrix.partial(at1, at2);

			// Subtract the simulated unweighted S(Q) from the difference partial
			partial.addInterpolated(unweightedSQ.partial(n, m), -1.0);

			// Perform some judicious smoothing on the data
			partial.smooth(10);
			partial.save("sub.sq");

			// Do the inverse FT
			partial.broadenedSineFT(1.0 / (2 * PI * rho), 0.05, 0.01, 30.0, broadening, true, XYData::HannWindow);
			partial.save("transformed.gr");
		}
	}

	/*
	 * Generate a correction to the potential
	 */
	const double maxInt = 0.25;
	double weight;
	n = 0;
	for (AtomType* at1 = duq.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++n)
	{
		int m = n;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++m)
		{
			// Grab difference partial, now back-transformed into a difference g(r)
			XYData& diffgr = differences.ref(n, m);

			// Process the addition a little?
			// TODO

			// Decide on the weight of the difference we will apply
			double absInt = diffgr.absIntegral();
			weight = absInt > maxInt ? -maxInt / absInt : -absInt;
			printf("ABSInt = %f, weight = %f\n", absInt, weight);

			// Grab and modify pair potential
			PairPotential* pp = duq.pairPotential(at1, at2);
			if (!pp)
			{
				Messenger::error("Failed to find PairPotential for AtomTypes '%s' and '%s'.\n", at1->name(), at2->name());
				return false;
			}
			pp->adjustUAdditional(diffgr, weight);
		}
	}

	return true;
}

// Execute post-processing stage
bool TestModule::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}
