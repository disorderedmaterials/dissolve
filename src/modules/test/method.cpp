/*
	*** Test Module - Method
	*** src/modules/test/method.cpp
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
	 * First, make sure that all of the associated Data are set up
	 */
	RefListIterator<Data,bool> dataIterator(targetData_);
	while (Data* data = dataIterator.iterate()) if (!data->setUp(duq.processingModuleData())) return false;

	/*
	 * Construct our full, square, scattering matrix, using the master AtomTypes list
	 */
	// Realise the generatedSQ array
	Array2D<XYData>& generatedSQ = GenericListHelper< Array2D<XYData> >::realise(duq.processingModuleData(), "GeneratedSQ", uniqueName_, GenericItem::InRestartFileFlag);
	ScatteringMatrix scatteringMatrix;
	scatteringMatrix.initialise(duq.atomTypeList(), generatedSQ, uniqueName_);

	// For each Data, get the Weights from the associated Partials module
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
	scatteringMatrix.generatePartials(generatedSQ);

	/*
	 * Construct difference matrix of partials
	 */
	int nTypes = duq.atomTypeList().nItems();
	bool created;
	Array2D<XYData>& deltaSQ = GenericListHelper< Array2D<XYData> >::realise(duq.processingModuleData(), "DeltaSQ", uniqueName_, GenericItem::InRestartFileFlag, &created);
	if (created) deltaSQ.initialise(nTypes, nTypes, true);
	int n = 0;
	for (AtomType* at1 = duq.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++n)
	{
		int m = n;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++m)
		{
			// Grab difference partial and make sure its object name is set
			XYData& partial = deltaSQ.ref(n, m);
			partial.setObjectName(CharString("%s//DeltaSQ//%s-%s", uniqueName_.get(), at1->name(), at2->name()));

			// Copy our partial S(Q) generated from the experimental datasets
			partial = generatedSQ.ref(n,m);

			// Subtract the simulated unweighted S(Q) from the difference partial
			partial.addInterpolated(unweightedSQ.partial(n, m), -1.0);
		}
	}

	/*
	 * Create perturbations to interatomic potentials
	 */
	TestModule::PotentialGenerationType generationType = TestModule::potentialGenerationType(keywords_.asString("PotentialGeneration"));
	const double weighting = keywords_.asDouble("Weighting");
	Array2D<XYData>& deltaPhiR = GenericListHelper< Array2D<XYData> >::realise(duq.processingModuleData(), "DeltaPhiR", uniqueName_, GenericItem::InRestartFileFlag, &created);
	if (created) deltaPhiR.initialise(nTypes, nTypes, true);
	Array2D<XYData>& deltaGR = GenericListHelper< Array2D<XYData> >::realise(duq.processingModuleData(), "DeltaGR", uniqueName_, GenericItem::InRestartFileFlag, &created);
	if (created) deltaGR.initialise(nTypes, nTypes, true);

	double weight, absInt;
	XYData cr;
	n = 0;
	for (AtomType* at1 = duq.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++n)
	{
		int m = n;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++m)
		{
			// Grab pointer to the relevant pair potential
			PairPotential* pp = duq.pairPotential(at1, at2);
			if (!pp)
			{
				Messenger::error("Failed to find PairPotential for AtomTypes '%s' and '%s'.\n", at1->name(), at2->name());
				return false;
			}

			// Grab potential perturbation container, clear it, and make sure its object name is set
			XYData& dPhiR = deltaPhiR.ref(n, m);
			dPhiR.clear();
			dPhiR.setObjectName(CharString("%s//DeltaPhiR//%s-%s", uniqueName_.get(), at1->name(), at2->name()));

			// Grab delta g(r) container and make sure its object name is set
			XYData& dGR = deltaGR.ref(n, m);
			dGR.setObjectName(CharString("%s//DeltaGR//%s-%s", uniqueName_.get(), at1->name(), at2->name()));

			// Copy the delta S(Q) and do the inverse FT to ger the delta [g(r) - 1]
			dGR = deltaSQ.ref(n,m);
// 			partial.smooth(10);
			dGR.broadenedSineFT(1.0 / (2 * PI * PI * rho), 0.0, 0.01, 30.0, broadening, true);

			// Set the default weighting factor for the pair potential addition
			weight = weighting;

			// Create a perturbation
			switch (generationType)
			{
				case (TestModule::DirectPotentialGeneration):
					// Decide on the weight of the difference we will apply
					absInt = dGR.absIntegral();
					weight = absInt > weight ? -weight / absInt : -absInt;
// 					printf("ABSInt = %f, weight = %f\n", absInt, weight);
					dPhiR = dGR;
					break;
				case (TestModule::PercusYevickPotentialGeneration):
					/*
					 * The original Percus-Yevick closure relationship for solving the Ornstein-Zernike equation is:
					 * 
					 *  phi(r) = ln(1.0 - c(r)/g(r))
					 * 
					 * The relationship assumes that the g(r) is well-behaved, but our delta g(r) may be anything but.
					 * Specifically, troughs in the delta g(r), which we interpret as indicating atoms need to be forced apart
					 * at these distances, are over-exagerrated compared to peaks because of the use of ln. Also, it is possible
					 * to have large dips in the delta g(r) that force the ratio c(r)/g(r) above 1.0, and create NaNs in the
					 * resulting function.
					 * 
					 * To account for this, we employ a slight modification of the relationship:
					 * 
					 * phi(r) = ln(1.0 - c(r) / [|g(r)-1.0| + 1.0])
					 * 
					 */

					// Calculate c(r) from the delta S(Q)
					cr = calculateCR(deltaSQ.ref(n,m), 1.0 / (2.0*PI*PI*rho), 0.0, 0.01, 30.0);
// 					cr.save("CR.txt");

					// dGR contains the FT of the delta S(Q), and oscillates around zero. 
					// Original PY
// 					for (int n=0; n<deltaGR.nPoints(); ++n) dU.addPoint(deltaGR.x(n), log(1.0 - cr.y(n)/(deltaGR.y(n)+1.0));

					// Modified PY
					for (int n=0; n<dGR.nPoints(); ++n) dPhiR.addPoint(dGR.x(n), log(1.0 - cr.y(n) / (fabs(dGR.y(n)-1.0)+1.0)));

// 					dU.save("py.txt");
// 					XYData hnc;
// 					for (int n=0; n<gr.nPoints(); ++n) hnc.addPoint(gr.x(n), (gr.y(n) - cr.y(n) - 1.0 - log(gr.y(n))));
// 					hnc.save("hnc.txt");
// 					return false;
					break;
				default:
					return false;
			}

			// Apply the perturbation
			dPhiR.arrayY() *= weight;
			pp->adjustUAdditional(dPhiR);


			// TEST
		// 	XYData gr = targetData_.firstItem()->data();
			// TEST - Normalise to 1.0 - this would already be the case for the predicted partials
		// 	gr.arrayY() /= 6.2001;
		// 	gr.save("SQ.txt");

// 			XYData cr = calculateCR(gr, 1.0 / (2.0*PI*PI*rho), 0.05, 0.01, 30.0);
// 			cr.save("CR.txt");
// 			gr.broadenedSineFT(1.0 / (2 * PI * PI * rho), 0.05, 0.01, 30.0, BroadeningFunction::unity(), false);
// 			gr.arrayY() += 1.0;
// 			gr.save("GR.txt");
// 			XYData py;
// 			for (int n=0; n<gr.nPoints(); ++n) py.addPoint(gr.x(n),  log(1.0 - cr.y(n)/gr.y(n)));
// 			py.save("py.txt");
// 			XYData hnc;
// 			for (int n=0; n<gr.nPoints(); ++n) hnc.addPoint(gr.x(n), (gr.y(n) - cr.y(n) - 1.0 - log(gr.y(n))));
// 			hnc.save("hnc.txt");
// 			return false;

		}
	}

	return true;
}

// Execute post-processing stage
bool TestModule::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}
