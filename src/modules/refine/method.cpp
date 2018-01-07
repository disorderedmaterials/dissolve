/*
	*** Refine Module - Method
	*** src/modules/refine/method.cpp
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

#include "modules/refine/refine.h"
#include "main/duq.h"
#include "classes/scatteringmatrix.h"
#include "classes/weights.h"
#include "classes/atomtype.h"
#include "base/sysfunc.h"
#include "templates/genericlisthelper.h"

// Perform set up tasks for module
bool RefineModule::setUp(ProcessPool& procPool)
{
	return true;
}

// Execute pre-processing stage
bool RefineModule::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Execute Method
bool RefineModule::process(DUQ& duq, ProcessPool& procPool)
{
	/*
	 * Grab dependent Module pointers
	 */
	Module* partialsModule = dependentModule("Partials");
	if (!partialsModule)
	{
		Messenger::error("No Partials Module associated to RefineModule '%s'.\n", uniqueName());
		return false;
	}

	/*
	 * Set Module data target
	 */
	GenericList& moduleData = configurationLocal_ ? targetConfigurations_.firstItem()->moduleData() : duq.processingModuleData();

	/*
	 * Get Keyword Options
	 */
	const double truncationWidth = keywords_.asDouble("TruncationWidth");
	const double minimumRadius = keywords_.asDouble("MinimumRadius");
	bool onlyWhenStable = keywords_.asBool("OnlyWhenStable");
	XYData::WindowFunction windowFunction = XYData::windowFunction(keywords_.asString("WindowFunction"));
	if (windowFunction == XYData::nWindowFunctions)
	{
		Messenger::error("Refine: Unrecognised window function '%s' found.\n", keywords_.asString("WindowFunction"));
		return false;
	}

	// Print option summary
	if (onlyWhenStable) Messenger::print("Refine: Potential refinement will only be attempted if all related Configuration energies are stable.\n");
	if (windowFunction == XYData::nWindowFunctions) Messenger::print("Refine: No window function will be employed in Fourier transforms.\n");
	else Messenger::print("Refine: '%s' window function will be employed in Fourier transforms.\n", XYData::windowFunction(windowFunction));

	/*
	 * Are the energies of all involved Configurations stable (if OnlyWhenStable option is on)
	 */
	if (onlyWhenStable)
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
					Messenger::print("Refine: Energy for Configuration '%s' is not yet stable. No potential refinement will be performed this iteration.\n", cfg->name());
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
	if (!found) Messenger::print("Refine: No 'QBroadening' specified in PartialsModule '%s', so no un-broadening will be performed.\n", partialsModule->uniqueName());
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
	 * Calculate current percentage errors in calculated vs experimental S(Q)
	 */
	dataIterator.restart();
	while (Data* data = dataIterator.iterate())
	{
		// Must have an associated Module from which to get the total S(Q)
		if (!data->associatedModule()) continue;

		// Retrieve the PartialSet from the module
		bool found = false;
		PartialSet& calcSQ = GenericListHelper<PartialSet>::retrieve(duq.processingModuleData(), "WeightedSQ", partialsModule->uniqueName(), PartialSet(), &found);
		if (!found)
		{
			Messenger::warn("Could not locate associated PartialSet for Data '%s'.\n", data->name());
			continue;
		}

		// Realise the error array and make sure its object name is set
		XYData& errors = GenericListHelper<XYData>::realise(duq.processingModuleData(), CharString("%s_Error", data->niceName()), uniqueName_, GenericItem::InRestartFileFlag);
		errors.setObjectName(CharString("%s//%s//Error", uniqueName_.get(), data->niceName()));

		// Calculate and store the percentage error
		errors.addPoint(duq.iteration(), data->data().error(calcSQ.total()));
	}

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
			Messenger::error("Refine: Couldn't find FullWeights for Data '%s', and so can't construct scattering matrix.\n", data->name());
			return false;
		}

		// Set the next row of the scattering matrix with the weights of the supplied data.
		if (!scatteringMatrix.addReferenceData(data, weights))
		{
			Messenger::error("Refine: Failed to initialise reference Data.\n");
			return false;
		}
	}
	if (!scatteringMatrix.finalise())
	{
		Messenger::error("Refine: Failed to set up scattering matrix.\n");
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
	int i = 0;
	for (AtomType* at1 = duq.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++i)
	{
		int j = i;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++j)
		{
			// Grab difference partial and make sure its object name is set
			XYData& partial = deltaSQ.ref(i, j);
			partial.setObjectName(CharString("%s//DeltaSQ//%s-%s", uniqueName_.get(), at1->name(), at2->name()));

			// Copy our partial S(Q) generated from the experimental datasets
			partial = generatedSQ.ref(i, j);

			// Subtract the simulated unweighted S(Q) from the difference partial
			partial.addInterpolated(unweightedSQ.partial(i, j), -1.0);
		}
	}

	/*
	 * Create perturbations to interatomic potentials
	 */
	RefineModule::PotentialGenerationType generationType = RefineModule::potentialGenerationType(keywords_.asString("PotentialGeneration"));
	const double weighting = keywords_.asDouble("Weighting");
	Array2D<XYData>& deltaPhiR = GenericListHelper< Array2D<XYData> >::realise(duq.processingModuleData(), "DeltaPhiR", uniqueName_, GenericItem::InRestartFileFlag, &created);
	if (created) deltaPhiR.initialise(nTypes, nTypes, true);
	Array2D<XYData>& deltaGR = GenericListHelper< Array2D<XYData> >::realise(duq.processingModuleData(), "DeltaGR", uniqueName_, GenericItem::InRestartFileFlag, &created);
	if (created) deltaGR.initialise(nTypes, nTypes, true);

	const double ppRange = duq.pairPotentialRange();
	const double ppDelta = duq.pairPotentialDelta();
	double weight, absInt, scaleFactor;
	XYData cr;
	i = 0;
	for (AtomType* at1 = duq.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++i)
	{
		int j = i;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++j)
		{
			// Grab pointer to the relevant pair potential
			PairPotential* pp = duq.pairPotential(at1, at2);
			if (!pp)
			{
				Messenger::error("Failed to find PairPotential for AtomTypes '%s' and '%s'.\n", at1->name(), at2->name());
				return false;
			}

			// Grab potential perturbation container, clear it, and make sure its object name is set
			XYData& dPhiR = deltaPhiR.ref(i, j);
			dPhiR.clear();
			dPhiR.setObjectName(CharString("%s//DeltaPhiR//%s-%s", uniqueName_.get(), at1->name(), at2->name()));

			// Grab delta g(r) container and make sure its object name is set
			XYData& dGR = deltaGR.ref(i, j);
			dGR.setObjectName(CharString("%s//DeltaGR//%s-%s", uniqueName_.get(), at1->name(), at2->name()));

			// Copy the delta S(Q) and do the inverse FT to get the delta [g(r) - 1]
			dGR = deltaSQ.ref(i, j);
			dGR.broadenedSineFT(1.0 / (2 * PI * PI * rho), ppDelta, ppDelta, ppRange, broadening, true, windowFunction);

			// Set the default weighting factor for the pair potential addition
			weight = weighting;

			// Create a perturbation
			switch (generationType)
			{
				case (RefineModule::DirectPotentialGeneration):
					// Decide on the weight of the difference we will apply
					absInt = dGR.absIntegral();
					weight = absInt > weight ? -weight / absInt : -absInt;
// 					printf("ABSInt = %f, weight = %f\n", absInt, weight);
					dPhiR = dGR;
					break;
				case (RefineModule::PercusYevickPotentialGeneration):
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
					 *  phi(r) = ln(1.0 - c(r) / [|g(r)-1.0| + 1.0])
					 * 
					 */

					// Calculate c(r) from the delta S(Q)
					cr = calculateCR(deltaSQ.ref(i, j), 1.0 / (2.0*PI*PI*rho), ppDelta, ppDelta, ppRange, BroadeningFunction::unity(), true, windowFunction);

					// dGR contains the FT of the delta S(Q), and oscillates around zero.
					// Scale the data to have a maximum deviation from zero of 1.0, so that we avoid any possibility to get NaNs when taking the ln later on
					scaleFactor = dGR.arrayY().maxAbs();
					if (scaleFactor < 1.0) scaleFactor = 1.0;
					dGR.arrayY() /= scaleFactor*2.0;
					cr.arrayY() /= scaleFactor*2.0;

					// Original PY
// 					for (int n=0; n<deltaGR.nPoints(); ++n) dU.addPoint(deltaGR.x(n), log(1.0 - cr.y(n)/(deltaGR.y(n)+1.0));

					// Modified PY
					for (int n=0; n<dGR.nPoints(); ++n) dPhiR.addPoint(dGR.x(n), log(1.0 - cr.y(n) / (fabs(dGR.y(n)-1.0)+1.0)));

					// Rescale the resulting potential to account for the reduction we made earlier
					dPhiR.arrayY() *= scaleFactor;

// 					dPhiR.save(CharString("py%i%i.txt", i, j));
// 					XYData hnc;
// 					for (int n=0; n<gr.nPoints(); ++n) hnc.addPoint(gr.x(n), (gr.y(n) - cr.y(n) - 1.0 - log(gr.y(n))));
// 					hnc.save("hnc.txt");
// 					return false;
					break;
				default:
					return false;
			}

			// Apply smooth zeroing of potential up to the minimum distance
			const double truncationStart = minimumRadius - truncationWidth;
			double r;
			Array<double>& y = dPhiR.arrayY();
			for (int n=0; n<dPhiR.nPoints(); ++n)
			{
				r = dPhiR.x(n);
				if (r < truncationStart) y[n] = 0.0;
				else if (r > minimumRadius) break;
				else y[n] *= 0.5 - 0.5*cos(PI*0.5*(r-truncationStart)/(truncationWidth*0.5));
			}

			// Apply the perturbation
			dPhiR.arrayY() *= weight;
			pp->adjustUAdditional(dPhiR);
		}
	}

	return true;
}

// Execute post-processing stage
bool RefineModule::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}
