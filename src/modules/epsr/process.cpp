/*
	*** EPSR Module - Processing
	*** src/modules/epsr/process.cpp
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

#include "modules/epsr/epsr.h"
#include "main/dissolve.h"
#include "modules/energy/energy.h"
#include "modules/rdf/rdf.h"
#include "math/error.h"
#include "math/gaussfit.h"
#include "math/poissonfit.h"
#include "classes/scatteringmatrix.h"
#include "classes/weights.h"
#include "classes/atomtype.h"
#include "classes/partialset.h"
#include "data/isotopes.h"
#include "base/sysfunc.h"
#include "templates/genericlisthelper.h"
#include "templates/array3d.h"

// Run set-up stage
bool EPSRModule::setUp(Dissolve& dissolve, ProcessPool& procPool)
{
	// If a pcof file was provided, read in the parameters from it here
	CharString pcofFile = keywords_.asString("PCofFile");
	if (!pcofFile.isEmpty())
	{
		// Read in the coefficients / setup from the supplied file
		if (!readPCof(dissolve, procPool, pcofFile)) return Messenger::error("Failed to read in potential coefficients from EPSR pcof file.\n");

		// Get some control parameters
		//const double ereq = keywords_.asDouble("EReq");
		int ncoeffp = keywords_.asInt("NCoeffP");
		double rmaxpt = keywords_.asDouble("RMaxPT");
		double rminpt = keywords_.asDouble("RMinPT");

		// Set up the additional potentials - reconstruct them from the current coefficients
		ExpansionFunctionType functionType = expansionFunctionType(keywords_.asString("ExpansionFunction"));
		if (functionType == EPSRModule::GaussianExpansionFunction)
		{
			const double gsigma1 = keywords_.asDouble("GSigma1");
			const double gsigma2 = keywords_.asDouble("GSigma2");
			if (!generateEmpiricalPotentials(dissolve, functionType, ncoeffp, rminpt, rmaxpt, gsigma1, gsigma2)) return false; 
		}
		else
		{
			const double psigma1 = keywords_.asDouble("PSigma1");
			const double psigma2 = keywords_.asDouble("PSigma2");
			if (!generateEmpiricalPotentials(dissolve, functionType, ncoeffp, rminpt, rmaxpt, psigma1, psigma2)) return false; 
		}
	}

	return true;
}

// Run main processing
bool EPSRModule::process(Dissolve& dissolve, ProcessPool& procPool)
{
	int i, j;
	CharString testDataName;

	/*
	 * Get Keyword Options
	 */
	const double ereq = keywords_.asDouble("EReq");
	ExpansionFunctionType functionType = expansionFunctionType(keywords_.asString("ExpansionFunction"));
	const double feedback = keywords_.asDouble("Feedback");
	const double gsigma1 = keywords_.asDouble("GSigma1");
	const double gsigma2 = keywords_.asDouble("GSigma2");
	const bool modifyPotential = keywords_.asBool("ModifyPotential");
	int ncoeffp = keywords_.asInt("NCoeffP");
	const int npitss = keywords_.asInt("NPItSs");
	const bool onlyWhenEnergyStable = keywords_.asBool("OnlyWhenEnergyStable");
	const double psigma1 = keywords_.asDouble("PSigma1");
	const double psigma2 = keywords_.asDouble("PSigma2");
	const double qMax = keywords_.asDouble("QMax");
	const double qMin = keywords_.asDouble("QMin");
	double rmaxpt = keywords_.asDouble("RMaxPT");
	double rminpt = keywords_.asDouble("RMinPT");
	const bool saveData = keywords_.asBool("Save");
	const bool testMode = keywords_.asBool("Test");
	const double testThreshold = keywords_.asDouble("TestThreshold");

	// EPSR constants
	const int mcoeff = 200;

	// Calculate some values if they were not provided
	if (rmaxpt < 0.0) rmaxpt = dissolve.pairPotentialRange();
	if (rminpt < 0.0) rminpt = rmaxpt - 2.0;
	if (ncoeffp <= 0) ncoeffp = std::min(int(10.0*rmaxpt+0.0001), mcoeff);

	// Print option summary
	Messenger::print("EPSR: Feedback factor is %f.\n", feedback);
	Messenger::print("EPSR: %s functions will be used to approximate difference data.\n", expansionFunctionType(functionType));
	Messenger::print("EPSR: Number of functions used in approximation is %i, sigma(Q) = %f.\n", ncoeffp, psigma2);
	if (modifyPotential) Messenger::print("EPSR: Perturbations to interatomic potentials will be generated and applied.\n");
	else Messenger::print("EPSR: Perturbations to interatomic potentials will be generated only (current potentials will not be modified).\n");
	if (onlyWhenEnergyStable) Messenger::print("EPSR: Potential refinement will only be performed if all related Configuration energies are stable.\n");
	Messenger::print("EPSR: Range for potential generation is %f < Q < %f Angstroms**-1.\n", qMin, qMax);
	Messenger::print("EPSR: Save data is %s.\n", DissolveSys::onOff(saveData));
	if (testMode) Messenger::print("EPSR: Test mode is enabled (threshold = %f%%).", testThreshold);
	Messenger::print("\n");


	/*
	 * Do we have targets to refine against?
	 */
	if (targets_.nItems() == 0) return Messenger::error("At least one Module target containing suitable data must be provided.\n");


	/*
	 * Make a list of all Configurations related to all targets
	 */
	RefList<Configuration,bool> configs;
	RefListIterator<Module,bool> allTargetsIterator(targets_);
	while (Module* module = allTargetsIterator.iterate())
	{
		RefListIterator<Configuration,bool> configIterator(module->targetConfigurations());
		while (Configuration* cfg = configIterator.iterate()) configs.addUnique(cfg);
	}
	Messenger::print("%i Configuration(s) are involved over all target data.\n", configs.nItems());


	/*
	 * Calculate difference functions and current percentage errors in calculated vs reference target data
	 */
	allTargetsIterator.restart();
	while (Module* module = allTargetsIterator.iterate())
	{
		// Realise the error array and make sure its object name is set
		XYData& errors = GenericListHelper<XYData>::realise(dissolve.processingModuleData(), CharString("Error_%s", module->uniqueName()), uniqueName_, GenericItem::InRestartFileFlag);
		errors.setObjectName(CharString("%s//Error//%s", uniqueName_.get(), module->uniqueName()));

		// Calculate our error based on the type of Module
		double error = 100.0;
		if (DissolveSys::sameString(module->name(), "NeutronSQ"))
		{
			bool found;

			// Retrieve the ReferenceData from the Module (as XYData)
			const XYData& referenceData = GenericListHelper<XYData>::value(dissolve.processingModuleData(), "ReferenceData", module->uniqueName(), XYData(), &found);
			if (!found)
			{
				Messenger::warn("Could not locate ReferenceData for target '%s'.\n", module->uniqueName());
				return false;
			}

			// Retrieve the PartialSet from the Module
			const PartialSet& calcSQ = GenericListHelper<PartialSet>::value(dissolve.processingModuleData(), "WeightedSQ", module->uniqueName(), PartialSet(), &found);
			if (!found)
			{
				Messenger::warn("Could not locate associated weighted neutron PartialSet for target '%s'.\n", module->uniqueName());
				return false;
			}
			XYData calcSQTotal = calcSQ.constTotal();

			error = Error::percent(referenceData, calcSQTotal);

			// Calculate difference
			XYData& differenceData = GenericListHelper<XYData>::realise(dissolve.processingModuleData(), CharString("DifferenceData_%s", module->uniqueName()), uniqueName(), GenericItem::InRestartFileFlag);
			differenceData.setObjectName(CharString("%s//Difference//%s", uniqueName_.get(), module->uniqueName()));
			differenceData = referenceData;
			Interpolator::addInterpolated(differenceData, calcSQTotal, -1.0);
		}
		else return Messenger::error("Unrecognised Module type '%s', so can't calculate error.", module->name());

		// Store the percentage error
		errors.addPoint(dissolve.iteration(), error);
		Messenger::print("Current error for reference data '%s' is %f%%.\n", module->uniqueName(), error);
	}


	/*
	 * Are the energies of all involved Configurations stable (if OnlyWhenEnergyStable option is on).
	 * We still need to determine current errors for reference data, so store the result and carry on.
	 */
	if (onlyWhenEnergyStable)
	{
		int stabilityResult = EnergyModule::checkStability(configs);
		if (stabilityResult == -1) return false;
		else if (stabilityResult != 0)
		{
			Messenger::print("At least one Configuration energy is not yet stable. No potential refinement will be performed this iteration.\n");
			return true;
		}
	}


	/*
	 * Calculate difference functions for each experimental dataset and fit them.
	 * Also calculate the simulated F(r) - for consistency with EPSR, this is the inverse FT of the simulated F(Q), rather than the directly-calculated function
	 */

	allTargetsIterator.restart();
	while (Module* module = allTargetsIterator.iterate())
	{
		bool found;

		// Retrieve the reference (experimental) data for the target Module
		const XYData& referenceData = GenericListHelper<XYData>::value(dissolve.processingModuleData(), "ReferenceData", module->uniqueName(), XYData(), &found);
		if (!found) return Messenger::error("Could not locate ReferenceData for target '%s'.\n", module->uniqueName());

		// Retrieve the simulated data for the target Module
		const PartialSet& weightedSQ = GenericListHelper<PartialSet>::value(dissolve.processingModuleData(), "WeightedSQ", module->uniqueName(), PartialSet(), &found);
		if (!found) return Messenger::error("Could not locate WeightedSQ for target '%s'.\n", module->uniqueName());

		// Get difference and fit function objects
		XYData& deltaFQ = GenericListHelper<XYData>::realise(dissolve.processingModuleData(), CharString("DeltaFQ_%s", module->uniqueName()), uniqueName_, GenericItem::InRestartFileFlag);
		XYData& deltaFQFit = GenericListHelper<XYData>::realise(dissolve.processingModuleData(), CharString("DeltaFQFit_%s", module->uniqueName()), uniqueName_, GenericItem::InRestartFileFlag);
		deltaFQ.setObjectName(CharString("%s//DeltaFQ//%s", uniqueName_.get(), module->uniqueName()));
		deltaFQFit.setObjectName(CharString("%s//DeltaFQFit//%s", uniqueName_.get(), module->uniqueName()));

		// Create the difference partial
		deltaFQ.clear();
		const Array<double> x1 = referenceData.constArrayX();
		const Array<double> y1 = referenceData.constArrayY();
		XYData simulatedFQ = weightedSQ.constTotal();
		Interpolator interpolatedSimFQ(simulatedFQ);

		// Determine allowable range for fit, based on requested values and limits of generated / simulated datasets.
		double deltaSQMin = qMin, deltaSQMax = (qMax < 0.0 ? x1.lastValue() : qMax);
		if ((deltaSQMin < x1.firstValue()) || (deltaSQMin < simulatedFQ.xMin())) deltaSQMin = max(x1.firstValue(), simulatedFQ.xMin());
		if ((deltaSQMax > x1.lastValue()) || (deltaSQMax > simulatedFQ.xMax())) deltaSQMax = min(x1.lastValue(), simulatedFQ.xMax());

		double x;
		for (int n=0; n<x1.nItems(); ++n)
		{
			// Grab experimental data x value
			x = x1.constAt(n);

			// If this x value is below the minimum Q value we are fitting, set the difference to zero. Otherwise, store the "inverse" value ([sim - exp], for consistency with EPSR)
			if (x < deltaSQMin) deltaFQ.addPoint(x, 0.0);
			else if (x > deltaSQMax) break;
			else deltaFQ.addPoint(x, interpolatedSimFQ.y(x) - y1.constAt(n));
		}

		/*
		 * Fit a function expansion to the deltaFQ
		 */

		// Do the coefficient arrays already exist? If so, we re-fit from this set of coefficients. If not, start from scratch
		bool created;
		Array<double>& fitCoefficients = GenericListHelper< Array<double> >::realise(dissolve.processingModuleData(), CharString("FitCoefficients_%s", module->uniqueName()), uniqueName_, GenericItem::InRestartFileFlag, &created);

		if (functionType == EPSRModule::GaussianExpansionFunction)
		{
			// Construct our fitting object
			GaussFit coeffMinimiser(deltaFQ);

			if (created) coeffMinimiser.constructReciprocal(0.0, rmaxpt, ncoeffp, gsigma1, npitss, 0.01, 10, 3, 3, false);
			else
			{
				if (fitCoefficients.nItems() != ncoeffp)
				{
					Messenger::warn("Number of terms (%i) in existing FitCoefficients array for target '%s' does not match the current number (%i), so will fit from scratch.\n", fitCoefficients.nItems(), module->uniqueName(), ncoeffp);
					coeffMinimiser.constructReciprocal(0.0, rmaxpt, ncoeffp, gsigma1, npitss, 0.01, 10, 3, 3, false);
				}
				else coeffMinimiser.constructReciprocal(0.0, rmaxpt, fitCoefficients, gsigma1, npitss, 0.01, 10, 3, 3, false);
			}

			// Store the new fit coefficients
			fitCoefficients = coeffMinimiser.A();

			deltaFQFit = coeffMinimiser.approximation();
		}
		else if (functionType == EPSRModule::PoissonExpansionFunction)
		{
			// Construct our fitting object
			PoissonFit coeffMinimiser(deltaFQ);

			if (created) coeffMinimiser.constructReciprocal(0.0, rmaxpt, ncoeffp, psigma1, psigma2, npitss, 0.01, 10, 3, 3, false);
			else
			{
				if (fitCoefficients.nItems() != ncoeffp)
				{
					Messenger::warn("Number of terms (%i) in existing FitCoefficients array for target '%s' does not match the current number (%i), so will fit from scratch.\n", fitCoefficients.nItems(), module->uniqueName(), ncoeffp);
					coeffMinimiser.constructReciprocal(0.0, rmaxpt, ncoeffp, psigma1, psigma2, npitss, 0.01, 10, 3, 3, false);
				}
				else coeffMinimiser.constructReciprocal(0.0, rmaxpt, fitCoefficients, psigma1, psigma2, npitss, 0.01, 100, 3, 3, false);
			}

			// Store the new fit coefficients
			fitCoefficients = coeffMinimiser.C();

			deltaFQFit = coeffMinimiser.approximation();
		}

		// Calculate F(r)
		XYData& simulatedFR = GenericListHelper<XYData>::realise(dissolve.processingModuleData(), "SimulatedFR", module->uniqueName(), GenericItem::InRestartFileFlag);
		simulatedFR.setObjectName(CharString("%s//SimulatedFR//%s", uniqueName_.get(), module->uniqueName()));
		simulatedFR = simulatedFQ;
		simulatedFR.sineFT(1.0 / (2*PI*PI*GenericListHelper<double>::value(dissolve.processingModuleData(), "EffectiveRho", module->uniqueName(), 0.0)), 0.0, 0.03, 30.0, WindowFunction(WindowFunction::Lorch0Window));

		// Test Mode
		if (testMode)
		{
			testDataName = CharString("WeightedFR-%s-total", module->uniqueName());
			if (testData_.contains(testDataName))
			{
				double error = Error::percent(simulatedFR, testData_.data(testDataName));
				Messenger::print("Simulated F(r) reference data '%s' has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", testDataName.get(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
				if (error > testThreshold) return false;
			}
		}
	}

	/*
	 * Loop over groups of defined Module targets.
	 * We will generate a contribution to dPhiR from each and blend them together.
	 */

	// Set up storage for the changes to coefficients used to generate the empirical potentials
	const int nAtomTypes = dissolve.nAtomTypes();
	Array3D<double> fluctuationCoefficients(nAtomTypes, nAtomTypes, ncoeffp);
	fluctuationCoefficients = 0.0;

	for (ModuleGroup* group = targetGroups_.first(); group != NULL; group = group->next)
	{
		Messenger::print("Generating dPhiR from target group '%s'...\n", group->name());

		// Grab Module list for this group and set up an iterator
		const RefList<Module,bool>& targetModules = group->modules();
		RefListIterator<Module,bool> targetIterator(targetModules);

		/*
		 * Create the full scattering matrix using all the reference (experimental) data we have.
		 */

		// Create temporary storage for our summed UnweightedSQ, and related quantities such as density and sum factors
		Array2D<XYData>& combinedUnweightedSQ = GenericListHelper< Array2D<XYData> >::realise(dissolve.processingModuleData(), "UnweightedSQ", uniqueName_, GenericItem::InRestartFileFlag);
		Array2D<double> combinedRho, combinedFactor, combinedCWeights;
		combinedUnweightedSQ.initialise(nAtomTypes, nAtomTypes, true);
		combinedRho.initialise(nAtomTypes, nAtomTypes, true);
		combinedFactor.initialise(nAtomTypes, nAtomTypes, true);
		combinedCWeights.initialise(nAtomTypes, nAtomTypes, true);
		combinedRho = 0.0;
		combinedFactor = 0.0;
		combinedCWeights = 0.0;

		// Set object names in combinedUnweightedSQ
		i = 0;
		for (AtomType* at1 = dissolve.atomTypes(); at1 != NULL; at1 = at1->next, ++i)
		{
			j = i;
			for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++j) combinedUnweightedSQ.at(i,j).setObjectName(CharString("%s//UnweightedSQ//%s//%s-%s", uniqueName(), group->name(), at1->name(), at2->name()));
		}

		// Realise storage for generated S(Q), and initialise a scattering matrix
		Array2D<XYData>& generatedSQ = GenericListHelper< Array2D<XYData> >::realise(dissolve.processingModuleData(), "GeneratedSQ", uniqueName_, GenericItem::InRestartFileFlag);
		ScatteringMatrix scatteringMatrix;
		scatteringMatrix.initialise(dissolve.atomTypeList(), generatedSQ, uniqueName_, group->name());

		// Add a row in the scattering matrix for each target in the group
		targetIterator.restart();
		while (Module* module = targetIterator.iterate())
		{
			bool found;

			// Retrieve the reference data, associated Weights matrix and source unweighted and weighted partials
			const XYData& referenceData = GenericListHelper<XYData>::value(dissolve.processingModuleData(), "ReferenceData", module->uniqueName(), XYData(), &found);
			if (!found) return Messenger::error("Could not locate ReferenceData for target '%s'.\n", module->uniqueName());

			Weights& weights = GenericListHelper<Weights>::retrieve(dissolve.processingModuleData(), "FullWeights", module->uniqueName(), Weights(), &found);
			if (!found) return Messenger::error("Could not locate Weights for target '%s'.\n", module->uniqueName());

			const PartialSet& unweightedSQ = GenericListHelper<PartialSet>::value(dissolve.processingModuleData(), "UnweightedSQ", module->uniqueName(), PartialSet(), &found);
			if (!found) return Messenger::error("Could not locate UnweightedSQ for target '%s'.\n", module->uniqueName());

			const PartialSet& weightedSQ = GenericListHelper<PartialSet>::value(dissolve.processingModuleData(), "WeightedSQ", module->uniqueName(), PartialSet(), &found);
			if (!found) return Messenger::error("Could not locate WeightedSQ for target '%s'.\n", module->uniqueName());

			double rho = GenericListHelper<
			double>::value(dissolve.processingModuleData(), "EffectiveRho", module->uniqueName(), 0.0, &found);
			if (!found) return Messenger::error("Could not locate EffectiveRho for target '%s'.\n", module->uniqueName());

			// Subtract intramolecular total from the reference data - this will enter into the ScatteringMatrix
			XYData refMinusIntra = referenceData, boundTotal = weightedSQ.boundTotal(false);
			Interpolator::addInterpolated(refMinusIntra, boundTotal, -1.0);

			// Add a row to our scattering matrix
			if (!scatteringMatrix.addReferenceData(refMinusIntra, weights, feedback)) return Messenger::error("Failed to add target data '%s' to weights matrix.\n", module->uniqueName());

			// Sum up the unweighted partials and density from this target
			double factor = 1.0;
			i = 0;
			for (AtomTypeData* atd1 = unweightedSQ.atomTypes().first(); atd1 != NULL; atd1 = atd1->next, ++i)
			{
				j = i;
				for (AtomTypeData* atd2 = atd1; atd2 != NULL; atd2 = atd2->next, ++j)
				{
					double globalI = atd1->atomType()->index();
					double globalJ = atd2->atomType()->index();

					XYData partialIJ = unweightedSQ.constUnboundPartial(i,j);
					Interpolator::addInterpolated(combinedUnweightedSQ.at(globalI, globalJ ), partialIJ, factor);
					combinedRho.at(globalI, globalJ) += rho * factor;
					combinedFactor.at(globalI, globalJ) += factor;
					combinedCWeights.at(globalI, globalJ) += weights.concentrationProduct(i,j);
				}
			}
		}

		// Normalise the combined values
		for (i=0; i<nAtomTypes; ++i)
		{
			for (j=i; j<nAtomTypes; ++j)
			{
				combinedUnweightedSQ.at(i,j) /= combinedFactor.constAt(i,j);
				combinedRho.at(i,j) /= combinedFactor.constAt(i,j);
				combinedCWeights.at(i,j) /= combinedFactor.constAt(i,j);
			}
		}

		/*
		 * Augment the scattering matrix.
		 */

		// Clear any existing simulated data
		simulatedReferenceData_.clear();

		// Create the array
		simulatedReferenceData_.createEmpty(combinedUnweightedSQ.linearArraySize());
		i = 0;
		for (AtomType* at1 = dissolve.atomTypes(); at1 != NULL; at1 = at1->next, ++i)
		{
			j = i;
			for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++j)
			{
				// Copy the unweighted data and wight weight it according to the natural isotope / concentration factor calculated above
				XYData data = combinedUnweightedSQ.at(i, j);
				data.setName(CharString("Simulated %s-%s", at1->name(), at2->name()));

				// Add this partial data to the scattering matrix - its factored weight will be (1.0 - feedback)
				if (!scatteringMatrix.addPartialReferenceData(data, at1, at2, 1.0, (1.0 - feedback))) return Messenger::error("EPSR: Failed to augment scattering matrix with partial %s-%s.\n", at1->name(), at2->name());
			}
		}

		scatteringMatrix.finalise();
		scatteringMatrix.print();

		if (Messenger::isVerbose())
		{
			Messenger::print("\nScattering Matrix Inverse:\n");
			scatteringMatrix.printInverse();

			Messenger::print("\nIdentity (Ainv * A):\n");
			scatteringMatrix.matrixProduct().print();
		}

		/*
		 * Generate S(Q) from completed scattering matrix
		 */

		scatteringMatrix.generatePartials(generatedSQ);

		// Save data?
		if (saveData)
		{
			XYData* generatedArray = generatedSQ.linearArray();
			for (int n=0; n<generatedSQ.linearArraySize(); ++n) generatedArray[n].save(generatedArray[n].name());
		}

		// Test Mode
		if (testMode)
		{
			i = 0;
			for (AtomType* at1 = dissolve.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++i)
			{
				j = i;
				for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++j)
				{
					testDataName = CharString("GeneratedSQ-%s-%s", at1->name(), at2->name());
					if (testData_.contains(testDataName))
					{
						double error = Error::percent(generatedSQ.at(i,j), testData_.data(testDataName));
						Messenger::print("Generated S(Q) reference data '%s' has error of %7.3f%% with calculated data and is %s (threshold is %6.3f%%)\n\n", testDataName.get(), error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
						if (error > testThreshold) return false;
					}
				}
			}
		}

		/*
		 * Calculate g(r) from generatedSQ
		 */

		Array2D<XYData>& generatedGR = GenericListHelper< Array2D<XYData> >::realise(dissolve.processingModuleData(), "GeneratedGR", uniqueName_, GenericItem::InRestartFileFlag);
		generatedGR.initialise(dissolve.atomTypeList().nItems(), dissolve.atomTypeList().nItems(), true);
		i = 0;
		for (AtomType* at1 = dissolve.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++i)
		{
			j = i;
			for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++j)
			{
				// Grab experimental g(r) container and make sure its object name is set
				XYData& expGR = generatedGR.at(i,j);
				expGR.setObjectName(CharString("%s//GeneratedGR//%s//%s-%s", uniqueName_.get(), group->name(), at1->name(), at2->name()));

				// Copy experimental S(Q) and FT it
				expGR = generatedSQ.at(i,j);
				expGR.sineFT(1.0 / (2 * PI * PI * combinedRho.at(i,j)), 0.0, 0.05, 30.0, WindowFunction(WindowFunction::Lorch0Window));
				expGR.arrayY() += 1.0;
			}
		}

		/*
		 * Generate summed unweighted g(r) from all source Module data
		 */

		PartialSet& summedUnweightedGR = GenericListHelper<PartialSet>::realise(dissolve.processingModuleData(), "UnweightedGR", uniqueName(), GenericItem::InRestartFileFlag);
		if (!RDFModule::sumUnweightedGR(procPool, this, group, dissolve.processingModuleData(), summedUnweightedGR)) return false;

		/*
		 * Calculate contribution to potential coefficients from this group.
		 * Multiply each coefficient by the associated weight in the inverse scattering matrix.
		 * Note: the data were added to the scattering matrix in the order they appear in the targets iterator.
		 */
		targetIterator.restart();
		int dataIndex = 0;
		while (Module* module = targetIterator.iterate())
		{
			// For this Module, retrive the coefficents of the fit performed above.
			const Array<double>& fitCoefficients = GenericListHelper< Array<double> >::value(dissolve.processingModuleData(), CharString("FitCoefficients_%s", module->uniqueName()), uniqueName_);
			
			// Loop over pair potentials and retrieve the inverse weight from the scattering matrix
			i = 0;
			for (AtomType* at1 = dissolve.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++i)
			{
				j = i;
				for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++j)
				{
					double weight = scatteringMatrix.pairWeightInverse(at1, at2, dataIndex);

					// Halve contribution from unlike terms to avoid adding double the potential for those partials
					if (i != j) weight *= 0.5;

					// Apply the (1.0/rho) factor here, since our expansion function normalisations don't contain it
					weight /= combinedRho.at(i,j);

					// Store fluctuation coefficients ready for addition to potential coefficients later on.
					for (int n=0; n<ncoeffp; ++n) fluctuationCoefficients.at(i, j, n) += weight * fitCoefficients.constAt(n);
				}
			}

			// Increase dataIndex
			++dataIndex;
		}
	}

	// Generate new empirical potentials
	double energabs = 0.0;
	if (modifyPotential)
	{
		// Sum fluctuation coefficients in to the potential coefficients
		Array2D< Array<double> >& coefficients = potentialCoefficients(dissolve, nAtomTypes, ncoeffp);

		i = 0;
		for (AtomType* at1 = dissolve.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++i)
		{
			j = i;
			for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++j)
			{
				Array<double>& potCoeff = coefficients.at(i, j);
				for (int n=0; n<ncoeffp; ++n) potCoeff[n] += fluctuationCoefficients.constAt(i, j, n);
			}
		}

		// Determine absolute energy of empirical potentials
		energabs = absEnergyEP(dissolve);

		/*
		 * Determine the scaling we will apply to the coefficients (if any)
		 * Notes:
		 * 	- ereq is actually read in as a variable called 'pressreq', then assigned to another variable 'absolute_energy' in the EPSR25 source.
		 * 	- erequnit appears to be set to the value of ereqstep read in from the 'ereqstep' command (clamped to 0.0-1.0).
		 */
		double pressfac = 1.0;
		double erequnit = 0.0, ereqstep = 0.0;

		if (fabs(ereq) == 0.0)
		{
			pressfac=0.0;
			energabs=0.0;
		}
		else if (fabs(energabs) > 0.0)
		{
			if (fabs(energabs) < fabs(erequnit)) pressfac = fabs(erequnit) / fabs(energabs);
			else
			{
				pressfac = fabs(ereq) / fabs(energabs);
				if ((pressfac > 1.0)  && (ereqstep == 0.0)) pressfac = 1.0;
			}
		}
		Messenger::print("  generate_ep>  %f  %f  %f\n", ereq, energabs, pressfac);

		// Scale coefficients
		for (i = 0; i<coefficients.linearArraySize(); ++i) coefficients.linearArray()[i] *= pressfac;
		energabs *= pressfac;
      
		// Generate additional potentials from the coefficients
		double sigma1 = functionType == EPSRModule::PoissonExpansionFunction ? psigma1 : gsigma1;
		double sigma2 = functionType == EPSRModule::PoissonExpansionFunction ? psigma2 : gsigma2;
		
		if (!generateEmpiricalPotentials(dissolve, functionType, ncoeffp, rminpt, rmaxpt, sigma1, sigma2)) return false;
	}
	else energabs = absEnergyEP(dissolve);

	// Realise the phiMag array and make sure its object name is set
	XYData& phiArray = GenericListHelper<XYData>::realise(dissolve.processingModuleData(), "EPMag", uniqueName_, GenericItem::InRestartFileFlag);
	phiArray.setObjectName(CharString("%s//EPMag", uniqueName_.get()));
	phiArray.addPoint(dissolve.iteration(), energabs);

	return true;
}

