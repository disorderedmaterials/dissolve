/*
	*** Refine Module - Processing
	*** src/modules/refine/process.cpp
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

// Run pre-processing stage
bool RefineModule::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Run main processing
bool RefineModule::process(DUQ& duq, ProcessPool& procPool)
{
	int i, j;

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
	const bool autoMinimumRadii = keywords_.asBool("AutoMinimumRadius");
	const bool smoothPhiR = keywords_.asBool("DeltaPhiRSmoothing");
	const int phiRSmoothK = keywords_.asDouble("DeltaPhiRSmoothK");
	const int phiRSmoothM = keywords_.asDouble("DeltaPhiRSmoothM");
	const bool smoothSQ = keywords_.asBool("DeltaSQSmoothing");
	const int sqSmoothK = keywords_.asDouble("DeltaSQSmoothK");
	const int sqSmoothM = keywords_.asDouble("DeltaSQSmoothM");
	const double globalMinimumRadius = keywords_.asDouble("MinimumRadius");
	const double globalMaximumRadius = keywords_.asDouble("MaximumRadius");
	const bool modifyBonds = keywords_.asBool("ModifyBonds");
	const bool modifyPotential = keywords_.asBool("ModifyPotential");
	bool onlyWhenStable = keywords_.asBool("OnlyWhenStable");
	double phiLimit = keywords_.asDouble("PhiLimit");
	const double truncationWidth = keywords_.asDouble("TruncationWidth");
	const WindowFunction& windowFunction = KeywordListHelper<WindowFunction>::retrieve(keywords_, "WindowFunction", WindowFunction());

	// Print option summary
	if (autoMinimumRadii) Messenger::print("Refine: Minimum radii of generated potentials will be determined automatically (min/max = %f/%f Angstroms).\n", globalMinimumRadius, globalMaximumRadius);
	else Messenger::print("Refine: Minimum radius of %f Angstroms will be applied to all generated potentials.\n", globalMinimumRadius);
	if (smoothSQ) Messenger::print("Refine: Generated difference S(Q) will be smoothed using KZ filter (k = %i, m = %i).\n", sqSmoothK, sqSmoothM);
	if (smoothPhiR) Messenger::print("Refine: Generated phi(r) will be smoothed using KZ filter (k = %i, m = %i).\n", phiRSmoothK, phiRSmoothM);
	if (modifyBonds) Messenger::print("Refine: Equilibrium master bond distances will be modified.\n");
	if (modifyPotential) Messenger::print("Refine: Perturbations to interatomic potentials will be generated and applied.\n");
	else Messenger::print("Refine: Perturbations to interatomic potentials will be generated only (current potentials will remain as-is).\n");
	if (onlyWhenStable) Messenger::print("Refine: Potential refinement will only be attempted if all related Configuration energies are stable.\n");
	if (phiLimit >= 0) Messenger::print("Refine: Limit of additional potential phi(r) across all potentials is %f kJ/mol/Angstrom.\n", phiLimit);
	else Messenger::warn("Refine: No limits will be applied to additional potential magnitude.\n");
	if (windowFunction.function() == WindowFunction::NoWindow) Messenger::print("Refine: No window function will be applied in Fourier transforms of S(Q) to g(r)");
	else Messenger::print("Refine: Window function to be applied in Fourier transforms of S(Q) to g(r) is %s (%s).", WindowFunction::functionType(windowFunction.function()), windowFunction.parameterSummary().get());

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

	BroadeningFunction broadening = KeywordListHelper<BroadeningFunction>::retrieve(partialsModule->keywords(), "QBroadening", BroadeningFunction());
	if (broadening.function() == BroadeningFunction::UnityFunction) Messenger::print("Refine: No 'QBroadening' specified in PartialsModule '%s', so no un-broadening will be performed.\n", partialsModule->uniqueName());
	else Messenger::print("Refine: QBroadening specified in PartialsModule '%s' (%s) will be removed when Fourier transforming the data.\n", partialsModule->uniqueName(), BroadeningFunction::functionType(broadening.function()));
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
		PartialSet& calcSQ = GenericListHelper<PartialSet>::retrieve(duq.processingModuleData(), "WeightedSQ", data->associatedModule()->uniqueName(), PartialSet(), &found);
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
		// TODO Need to take care that added reference data span consistent ranges in Q
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
	 * Calculate g(r) from generatedSQ
	 */
	Array2D<XYData>& generatedGR = GenericListHelper< Array2D<XYData> >::realise(duq.processingModuleData(), "GeneratedGR", uniqueName_, GenericItem::InRestartFileFlag);
	generatedGR.initialise(duq.atomTypeList().nItems(), duq.atomTypeList().nItems(), true);
	i = 0;
	for (AtomType* at1 = duq.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++i)
	{
		j = i;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++j)
		{
			// Grab experimental g(r) contained and make sure its object name is set
			XYData& expGR = generatedGR.ref(i,j);
			expGR.setObjectName(CharString("%s//GeneratedGR//%s-%s", uniqueName_.get(), at1->name(), at2->name()));

			// Copy experimental S(Q) and FT it
			expGR = generatedSQ.ref(i,j);
			expGR.sineFT(1.0 / (2 * PI * PI * rho), 0.0, 0.05, 30.0, WindowFunction(WindowFunction::Lorch0Window));
			expGR.arrayY() += 1.0;
		}
	}

	/*
	 * Construct difference matrix of partials
	 */
	int nTypes = duq.atomTypeList().nItems();
	bool created;
	Array2D<XYData>& deltaSQ = GenericListHelper< Array2D<XYData> >::realise(duq.processingModuleData(), "DeltaSQ", uniqueName_, GenericItem::InRestartFileFlag, &created);
	if (created) deltaSQ.initialise(nTypes, nTypes, true);

	i = 0;
	for (AtomType* at1 = duq.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++i)
	{
		j = i;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++j)
		{
			// Grab difference partial and make sure its object name is set
			XYData& dSQ = deltaSQ.ref(i, j);
			dSQ.setObjectName(CharString("%s//DeltaSQ//%s-%s", uniqueName_.get(), at1->name(), at2->name()));

			// Reset the difference partial
			dSQ.clear();

			// Create the difference partial, over the range permitted by both datasets
			const Array<double> x1 = generatedSQ.ref(i, j).constArrayX();
			const Array<double> y1 = generatedSQ.ref(i, j).constArrayY();
			XYData& simulatedSQ = unweightedSQ.partial(i,j);
			const double x2min = 0.5; //simulatedSQ.xFirst();
			const double x2max = simulatedSQ.xLast();

			double x;
			for (int n=0; n<x1.nItems(); ++n)
			{
				x = x1.value(n);
				if (x < x2min) continue;
				if (x > x2max) break;

				dSQ.addPoint(x, y1.value(n) - simulatedSQ.interpolated(x));
			}

			// Generate smeared representation of difference S(Q)
			if (smoothSQ) dSQ.kolmogorovZurbenkoFilter(sqSmoothK, sqSmoothM);
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
	if (modifyBonds)
	{
		Array2D<XYData>& deltaGRBond = GenericListHelper< Array2D<XYData> >::realise(duq.processingModuleData(), "DeltaGRBond", uniqueName_, GenericItem::InRestartFileFlag, &created);
		if (created) deltaGRBond.initialise(nTypes, nTypes, true);
	}

	// Set up / updated minimum radii array
	Array2D<double>& minimumRadii = GenericListHelper< Array2D<double> >::realise(duq.processingModuleData(), "MinimumRadii", uniqueName_, GenericItem::InRestartFileFlag, &created);
	if (created) minimumRadii.initialise(nTypes, nTypes, true);
	if (autoMinimumRadii)
	{
		// Get calculated g(r), from which we will determine the minimum radius to employ in the generated pair potential
		bool found;
		PartialSet& unweightedGR = GenericListHelper<PartialSet>::retrieve(duq.processingModuleData(), "UnweightedGR", partialsModule->uniqueName(), PartialSet(), &found);
		if (!found)
		{
			Messenger::error("Couldn't locate 'UnweightedGR' data in PartialsModule '%s'.\n", partialsModule->uniqueName());
			return false;
		}

		// Define a fraction of the determined g(r) non-zero point that will become our radius limit
		const double rFraction = 0.95;
		const double thresholdValue = 0.1;
		i = 0;
		for (AtomType* at1 = duq.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++i)
		{
			j = i;
			for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++j)
			{
				// Grab unbound g(r)
				XYData& gr = unweightedGR.unboundPartial(i, j);

				// Find first non-zero (above the threshold value) point in g(r)
				int n;
				for (n=0; n<gr.nPoints(); ++n) if (gr.y(n) > thresholdValue) break;
				if (n < gr.nPoints())
				{
					double x = gr.x(n) * rFraction;
					if (x < globalMinimumRadius) minimumRadii.ref(i,j) = globalMinimumRadius;
					else if (x < globalMaximumRadius) minimumRadii.ref(i,j) = x;
					else minimumRadii.ref(i,j) = globalMaximumRadius;
					
				}
				else minimumRadii.ref(i,j) = globalMaximumRadius;

				Messenger::print("Minimum radius for %s-%s interatomic potential determined to be %f Angstroms.\n", at1->name(), at2->name(), minimumRadii.value(i,j));
			}
		}
	}
	else minimumRadii = globalMinimumRadius;
	
	const double ppRange = duq.pairPotentialRange();
	const double ppDelta = duq.pairPotentialDelta();
	double weight, absInt, scaleFactor;
	XYData cr;
	Array<double> crgr;
	i = 0;
	for (AtomType* at1 = duq.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++i)
	{
		j = i;
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
			dGR.sineFT(1.0 / (2 * PI * PI * rho), ppDelta, ppDelta, ppRange, windowFunction);

			// Set the default weighting factor for the pair potential addition
			weight = weighting;

			// If we are modifying bond equilibrium distances, search for signatures in the deltaGR, do this now.
			// Such signatures will be removed from the final dGR, and will thus not contribute to the PairPotential addition.
			if (modifyBonds)
			{
				// Grab the deltaGRBond container and make sure its object name is set
				Array2D<XYData>& deltaGRBonds = GenericListHelper< Array2D<XYData> >::retrieve(duq.processingModuleData(), "DeltaGRBond", uniqueName_);
				XYData& deltaGRBond = deltaGRBonds.ref(i,j);
				deltaGRBond.setObjectName(CharString("%s//DeltaGRBond//%s-%s", uniqueName_.get(), at1->name(), at2->name()));

				if (!modifyBondTerms(duq, dGR, at1, at2, deltaGRBond)) return false;
				dGR.addInterpolated(deltaGRBond, -1.0);
			}

			// Create a perturbation
			switch (generationType)
			{
				case (RefineModule::DirectPotentialGeneration):
					// Decide on the weight of the difference we will apply
					absInt = dGR.absIntegral();
// 					weight = absInt > weight ? -weight / absInt : -absInt;
// // 					printf("ABSInt = %f, weight = %f\n", absInt, weight);
					dPhiR = dGR;
					dPhiR.arrayY() *= -1.0;
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
					cr = calculateCR(deltaSQ.ref(i, j), 1.0 / (2.0*PI*PI*rho), ppDelta, ppDelta, ppRange, windowFunction);

					// dGR contains the FT of the delta S(Q), and oscillates around zero.
					// Scale the data to have a maximum deviation from zero of 1.0, so that we avoid any possibility to get NaNs when taking the ln later on

					for (int n=0; n<dGR.nPoints(); ++n) crgr.add(cr.y(n) / (fabs(dGR.y(n)-1.0)+1.0));
					
					scaleFactor = crgr.maxAbs() * 1.01;
					if (scaleFactor < 1.0) scaleFactor = 1.01;
// 					dGR.arrayY() /= scaleFactor;
// 					cr.arrayY() /= scaleFactor;
					crgr /= scaleFactor;

					// Original PY
// 					for (int n=0; n<deltaGR.nPoints(); ++n) dU.addPoint(deltaGR.x(n), log(1.0 - cr.y(n)/(deltaGR.y(n)+1.0));

					// Modified PY
					for (int n=0; n<dGR.nPoints(); ++n) dPhiR.addPoint(dGR.x(n), log(1.0 - crgr[n])); //cr.y(n) / (fabs(dGR.y(n)-1.0)+1.0)));

					// Rescale the resulting potential to account for the reduction we made earlier
					dPhiR.arrayY() *= scaleFactor;
					break;
				case (RefineModule::HypernettedChainPotentialGeneration):
					/*
					 * The original hypernetted chain closure relationship for solving the Ornstein-Zernike equation is:
					 * 
					 *  phi(r) = g(r) - c(r) - 1.0 - ln(g(r))
					 * 
					 * As with the Percus-Yevick closure above, we must adjust the relationship slightly...
					 * 
					 */

					// Calculate c(r) from the delta S(Q)
					cr = calculateCR(deltaSQ.ref(i, j), 1.0 / (2.0*PI*PI*rho), ppDelta, ppDelta, ppRange, windowFunction, BroadeningFunction(), true);

					scaleFactor = dGR.arrayY().maxAbs() * 0.75;
					if (scaleFactor < 1.0) scaleFactor = 1.0;
					dGR.arrayY() /= scaleFactor;
					cr.arrayY() /= scaleFactor;

					for (int n=0; n<dGR.nPoints(); ++n) dPhiR.addPoint(dGR.x(n), dGR.y(n) - cr.y(n) - 1.0 - log(dGR.y(n) + 1.0));
					break;
				default:
					return false;
			}

			// Apply smooth zeroing of potential up to the minimum distance, and truncate at the end
			double minimumRadius = minimumRadii.value(i,j);
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

			// Smooth phi(r)
			if (smoothPhiR) dPhiR.kolmogorovZurbenkoFilter(phiRSmoothK, phiRSmoothM);

			// Make sure we go smoothly to zero at the limit of the potential
			for (int n=0; n<dPhiR.nPoints(); ++n) dPhiR.multiplyY(n, 1.0 - double(n)/(dPhiR.nPoints()-1));

			// Apply the perturbation to the PairPotential
			if (modifyPotential)
			{
				dPhiR.arrayY() *= weight;
				pp->adjustUAdditional(dPhiR);
			}
		}
	}

	// Clamp magnitude of additional potentials if required
	if (modifyPotential && (phiLimit > 0.0))
	{
		double phiMag = 0.0;
		i = 0;
		for (AtomType* at1 = duq.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++i)
		{
			j = i;
			for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++j)
			{
				// Grab pointer to the relevant pair potential
				PairPotential* pp = duq.pairPotential(at1, at2);
				if (pp) phiMag += pp->uAdditional().absIntegral();
			}
		}

		if (phiMag > phiLimit)
		{
			double factor = phiLimit / phiMag;

			i = 0;
			for (AtomType* at1 = duq.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++i)
			{
				j = i;
				for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++j)
				{
					// Grab pointer to the relevant pair potential
					PairPotential* pp = duq.pairPotential(at1, at2);
					if (pp) pp->uAdditional().arrayY() *= factor;
				}
			}
		}
	}

	return true;
}

// Run post-processing stage
bool RefineModule::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}
