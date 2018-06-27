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
#include "math/gaussfit.h"
#include "math/poissonfit.h"
#include "classes/scatteringmatrix.h"
#include "classes/weights.h"
#include "classes/atomtype.h"
#include "classes/partialset.h"
#include "base/sysfunc.h"
#include "templates/genericlisthelper.h"

// Return whether the Module has a processing stage
bool EPSRModule::hasProcessing()
{
	return true;
}

// Run main processing
bool EPSRModule::process(Dissolve& dissolve, ProcessPool& procPool)
{
	int i, j;
	bool found;

	/*
	 * Set Module data target
	 */
	GenericList& moduleData = configurationLocal_ ? targetConfigurations_.firstItem()->moduleData() : dissolve.processingModuleData();

	/*
	 * Get Keyword Options
	 */
	const double feedback = keywords_.asDouble("Feedback");
	const double ereq = keywords_.asDouble("EReq");
	const double errorStabilityThreshold = keywords_.asDouble("ErrorStabilityThreshold");
	const int errorStabilityWindow = keywords_.asInt("ErrorStabilityWindow");
	ExpansionFunctionType functionType = expansionFunctionType(keywords_.asString("ExpansionFunction"));
	const double globalMinimumRadius = keywords_.asDouble("MinimumRadius");
	const double globalMaximumRadius = keywords_.asDouble("MaximumRadius");
	const bool modifyPotential = keywords_.asBool("ModifyPotential");
	int ncoeffp = keywords_.asInt("NCoeffP");
	const int npitss = keywords_.asInt("NPItSs");
	const bool onlyWhenEnergyStable = keywords_.asBool("OnlyWhenEnergyStable");
	const bool onlyWhenErrorStable = keywords_.asBool("OnlyWhenErrorStable");
	const double psigma2 = keywords_.asDouble("PSigma2");
	const double qMax = keywords_.asDouble("QMax");
	const double qMin = keywords_.asDouble("QMin");
	const double rmaxpt = keywords_.asDouble("RMaxPT");
	const double rminpt = keywords_.asDouble("RMinPT");

	// EPSR constants
	const int mcoeff = 200;

	// Calculate some values if they were not provided
	if (ncoeffp <= 0) ncoeffp = std::min(int(10.0*rmaxpt+0.0001), mcoeff);

	// Print option summary
	Messenger::print("EPSR: Feedback factor is %f.\n", feedback);
	Messenger::print("EPSR: %s functions will be used to approximate difference functions.\n", expansionFunctionType(functionType));
	Messenger::print("EPSR: Number of functions used in approximation is %i, sigma(Q) = %f.\n", ncoeffp, psigma2);
// 	else Messenger::print("EPSR: Minimum radius of %f Angstroms will be applied to all generated potentials.\n", globalMinimumRadius);
	if (modifyPotential) Messenger::print("EPSR: Perturbations to interatomic potentials will be generated and applied.\n");
	else Messenger::print("EPSR: Perturbations to interatomic potentials will be generated only (current potentials will not be modified).\n");
	if (onlyWhenEnergyStable) Messenger::print("EPSR: Potential refinement will only be performed if all related Configuration energies are stable.\n");
	if (onlyWhenErrorStable) Messenger::print("EPSR: Potential refinement will only be performed if all percentage errors with reference data are stable.\n");
	Messenger::print("EPSR: Range for potential generation is %f < Q < %f Angstroms**-1.\n", qMin, qMax);
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
	 * Calculate current percentage errors in calculated vs reference target data
	 */
	int nUnstableData = 0;
	for (ModuleGroup* group = targetGroups_.first(); group != NULL; group = group->next)
	{
		// Grab Module list for this group and set up an iterator
		const RefList<Module,bool>& targetModules = group->modules();
		RefListIterator<Module,bool> targetIterator(targetModules);
		while (Module* module = targetIterator.iterate())
		{
			// Realise the error array and make sure its object name is set
			XYData& errors = GenericListHelper<XYData>::realise(dissolve.processingModuleData(), CharString("Error_%s", module->uniqueName()), uniqueName_, GenericItem::InRestartFileFlag);
			errors.setObjectName(CharString("%s//Error//%s", uniqueName_.get(), module->uniqueName()));

			// Calculate our error based on the type of Module
			double error = 100.0;
			if (DissolveSys::sameString(module->name(), "NeutronSQ"))
			{
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

				error = referenceData.error(calcSQTotal);

				// Calculate difference
				XYData& differenceData = GenericListHelper<XYData>::realise(dissolve.processingModuleData(), CharString("DifferenceData_%s", module->uniqueName()), uniqueName());
				differenceData.setObjectName(CharString("%s//Difference//%s", uniqueName_.get(), module->uniqueName()));
				differenceData = referenceData;
				differenceData.addInterpolated(calcSQTotal, -1.0);
			}
			else return Messenger::error("Unrecognised Module type '%s', so can't calculate error.", module->name());

			// Store the percentage error
			errors.addPoint(dissolve.iteration(), error);
			Messenger::print("Current error for reference data '%s' is %f%%.\n", module->uniqueName(), error);

			// Assess the stability of the current error
			if (errorStabilityWindow > errors.nPoints()) ++nUnstableData;
			else
			{
				double yMean;
				double grad = errors.lastGradient(errorStabilityWindow, &yMean);
				double thresholdValue = fabs(errorStabilityThreshold*yMean);
				if (fabs(grad) > thresholdValue) ++nUnstableData;

				Messenger::print("Error gradient of last %i points for reference data '%s' is %e %%/step (absolute threshold value is %e, stable = %s).\n", errorStabilityWindow, module->uniqueName(), grad, thresholdValue, DissolveSys::btoa(fabs(grad) <= thresholdValue));
			}
		}
	}

	// Check error stability if requested
	if (onlyWhenErrorStable && (nUnstableData > 0))
	{
		Messenger::print("Errors for %i reference datasets are unstable, so no potential refinement will be performed this iteration.\n", nUnstableData);
		return true;
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
	 * Loop over groups of defined Module targets.
	 * We will generate a contribution to dPhiR from each and blend them together.
	 */
	const int nTypes = dissolve.nAtomTypes();
	Array2D<double> globalCombinedErrors;
	globalCombinedErrors.initialise(dissolve.nAtomTypes(), dissolve.nAtomTypes(), true);
	globalCombinedErrors = 0.0;
	bool created;
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
		combinedUnweightedSQ.initialise(dissolve.nAtomTypes(), dissolve.nAtomTypes(), true);
		combinedRho.initialise(dissolve.nAtomTypes(), dissolve.nAtomTypes(), true);
		combinedFactor.initialise(dissolve.nAtomTypes(), dissolve.nAtomTypes(), true);
		combinedCWeights.initialise(dissolve.nAtomTypes(), dissolve.nAtomTypes(), true);
		combinedRho = 0.0;
		combinedFactor = 0.0;
		combinedCWeights = 0.0;

		// Set object names in combinedUnweightedSQ
		i = 0;
		for (AtomType* at1 = dissolve.atomTypes(); at1 != NULL; at1 = at1->next, ++i)
		{
			j = i;
			for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++j) combinedUnweightedSQ.ref(i,j).setObjectName(CharString("%s//UnweightedSQ//%s//%s-%s", uniqueName(), group->name(), at1->name(), at2->name()));
		}

		// Realise storage for generated S(Q), and reinitialise the scattering matrix
		Array2D<XYData>&  generatedSQ = GenericListHelper< Array2D<XYData> >::realise(dissolve.processingModuleData(), "GeneratedSQ", uniqueName_, GenericItem::InRestartFileFlag);
		scatteringMatrix_.initialise(dissolve.atomTypeList(), generatedSQ, uniqueName_, group->name());

		// Add a row in the scattering matrix for each target in the group
		targetIterator.restart();
		while (Module* module = targetIterator.iterate())
		{
			// Retrieve the reference data and associated Weights matrix and source unweighted partials
			const XYData& referenceData = GenericListHelper<XYData>::value(dissolve.processingModuleData(), "ReferenceData", module->uniqueName(), XYData(), &found);
			if (!found) return Messenger::error("Could not locate ReferenceData for target '%s'.\n", module->uniqueName());
			Weights& weights = GenericListHelper<Weights>::retrieve(dissolve.processingModuleData(), "FullWeights", module->uniqueName(), Weights(), &found);
			if (!found) return Messenger::error("Could not locate Weights for target '%s'.\n", module->uniqueName());
			const PartialSet& unweightedSQ = GenericListHelper<PartialSet>::value(dissolve.processingModuleData(), "UnweightedSQ", module->uniqueName(), PartialSet(), &found);
			if (!found) return Messenger::error("Could not locate UnweightedSQ for target '%s'.\n", module->uniqueName());
			double rho = GenericListHelper<
			double>::value(dissolve.processingModuleData(), "EffectiveRho", module->uniqueName(), 0.0, &found);
			if (!found) return Messenger::error("Could not locate EffectiveRho for target '%s'.\n", module->uniqueName());

			// Add a row to our scattering matrix
			if (!scatteringMatrix_.addReferenceData(referenceData, weights, feedback)) return Messenger::error("Failed to add target data '%s' to weights matrix.\n", module->uniqueName());

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

					XYData partialIJ = unweightedSQ.constPartial(i,j);
					combinedUnweightedSQ.ref(globalI, globalJ ).addInterpolated(partialIJ, factor);
					combinedRho.ref(globalI, globalJ) += rho * factor;
					combinedFactor.ref(globalI, globalJ) += factor;
					combinedCWeights.ref(globalI, globalJ) += weights.concentrationWeight(i,j);
				}
			}
		}

		// Normalise the combined values
		for (i=0; i<dissolve.nAtomTypes(); ++i)
		{
			for (j=i; j<dissolve.nAtomTypes(); ++j)
			{
				combinedUnweightedSQ.ref(i,j) /= combinedFactor.value(i,j);
				combinedRho.ref(i,j) /= combinedFactor.value(i,j);
				combinedCWeights.ref(i,j) /= combinedFactor.value(i,j);
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
				// Weight in the matrix will be based on the natural isotope and the summed concentration weight
				double factor = PeriodicTable::element(at1->element()).isotope(Isotope::NaturalIsotope)->boundCoherent() * PeriodicTable::element(at2->element()).isotope(Isotope::NaturalIsotope)->boundCoherent() * 0.01;
				factor *= combinedCWeights.value(i,j);

				// Copy the unweighted data and wight weight it according to the natural isotope / concentration factor calculated above
				XYData data = combinedUnweightedSQ.ref(i, j);
				data.arrayY() *= factor;
				data.setName(CharString("Simulated %s-%s", at1->name(), at2->name()));

				// Add this partial data to the scattering matrix
				if (!scatteringMatrix_.addPartialReferenceData(data, at1, at2, factor, (1.0 - feedback))) return Messenger::error("EPSR: Failed to augment scattering matrix with partial %s-%s.\n", at1->name(), at2->name());
			}
		}

		scatteringMatrix_.finalise();
		scatteringMatrix_.print();

		/*
		 * Generate S(Q) from completed scattering matrix
		 */

		// Generate the scattering matrix 
		scatteringMatrix_.generatePartials(generatedSQ);

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
				// Grab experimental g(r) contained and make sure its object name is set
				XYData& expGR = generatedGR.ref(i,j);
				expGR.setObjectName(CharString("%s//GeneratedGR//%s//%s-%s", uniqueName_.get(), group->name(), at1->name(), at2->name()));

				// Copy experimental S(Q) and FT it
				expGR = generatedSQ.ref(i,j);
				expGR.sineFT(1.0 / (2 * PI * PI * combinedRho.ref(i,j)), 0.0, 0.05, 30.0, WindowFunction(WindowFunction::Lorch0Window));
				expGR.arrayY() += 1.0;
			}
		}

		/*
		 * Generate summed unweighted g(r) from all source Module data
		 */

		PartialSet& summedUnweightedGR = GenericListHelper<PartialSet>::realise(dissolve.processingModuleData(), "UnweightedGR", uniqueName(), GenericItem::InRestartFileFlag);
		if (!RDFModule::sumUnweightedGR(procPool, this, group, dissolve.processingModuleData(), summedUnweightedGR)) return false;

		/*
		 * Construct matrix of difference partials (deltaSQ) for all AtomTypes
		 */
		Array2D<XYData>& deltaSQ = GenericListHelper< Array2D<XYData> >::realise(dissolve.processingModuleData(), "DeltaSQ", uniqueName_, GenericItem::InRestartFileFlag, &created);
		if (created) deltaSQ.initialise(nTypes, nTypes, true);

		i = 0;
		for (AtomType* at1 = dissolve.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++i)
		{
			j = i;
			for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++j)
			{
				// Grab difference partial and make sure its object name is set
				XYData& dSQ = deltaSQ.ref(i, j);
				dSQ.setObjectName(CharString("%s//DeltaSQ//%s//%s-%s", uniqueName_.get(), group->name(), at1->name(), at2->name()));

				// Reset the difference partial
				dSQ.clear();

				// Create the difference partial
				const Array<double> x1 = generatedSQ.ref(i, j).constArrayX();
				const Array<double> y1 = generatedSQ.ref(i, j).constArrayY();
				XYData& simulatedSQ = combinedUnweightedSQ.ref(i,j);

				// Determine allowable range for fit, based on requested values and limits of generated / simulated datasets
				double deltaSQMin = qMin, deltaSQMax = (qMax < 0.0 ? x1.last() : qMax);
				if ((deltaSQMin < x1.first()) || (deltaSQMin < simulatedSQ.xFirst())) deltaSQMin = max(x1.first(), simulatedSQ.xFirst());
				if ((deltaSQMax > x1.last()) || (deltaSQMax > simulatedSQ.xLast())) deltaSQMax = min(x1.last(), simulatedSQ.xLast());

				XYData refSQTrimmed;
				double x;
				for (int n=0; n<x1.nItems(); ++n)
				{
					x = x1.value(n);
					if (x < deltaSQMin) continue;
					if (x > deltaSQMax) break;
					refSQTrimmed.addPoint(x, y1.value(n));

					dSQ.addPoint(x, y1.value(n) - simulatedSQ.interpolated(x));
				}

				// Calculate current error between experimental and simulation partials and sum it into our array
				globalCombinedErrors.ref(i, j) += refSQTrimmed.error(simulatedSQ);
			}
		}


		/*
		 * For each experimental dataset, form the delta F(Q) and fit a function series to it.
		 * This generates the C(i) coefficients from the Q-space data.
		 */

		Array<XYData>& groupDeltaFQ = GenericListHelper< Array<XYData> >::realise(dissolve.processingModuleData(), CharString("DeltaFQ_%s", group->name()), uniqueName_, GenericItem::InRestartFileFlag, &created);
		if (created) groupDeltaFQ.initialise(targetModules.nItems());
		Array<XYData>& groupDeltaFQFit = GenericListHelper< Array<XYData> >::realise(dissolve.processingModuleData(), CharString("DeltaFQFit_%s", group->name()), uniqueName_, GenericItem::InRestartFileFlag, &created);
		if (created) groupDeltaFQFit.initialise(targetModules.nItems());

		targetIterator.restart();
		int index = 0;
		while (Module* module = targetIterator.iterate())
		{
			// Retrieve the reference (experimental) data for the target Module
			const XYData& referenceData = GenericListHelper<XYData>::value(dissolve.processingModuleData(), "ReferenceData", module->uniqueName(), XYData(), &found);
			if (!found) return Messenger::error("Could not locate ReferenceData for target '%s'.\n", module->uniqueName());

			// Retrieve the simulated data for the target Module
			const PartialSet& weightedSQ = GenericListHelper<PartialSet>::value(dissolve.processingModuleData(), "WeightedSQ", module->uniqueName(), PartialSet(), &found);
			if (!found) return Messenger::error("Could not locate WeightedSQ for target '%s'.\n", module->uniqueName());

			// Grab difference partial and make sure its object name is set
			XYData& dFQ = groupDeltaFQ[index];
			dFQ.setObjectName(CharString("%s//DeltaFQ//%s//%s", uniqueName_.get(), group->name(), module->uniqueName()));

			// Reset the difference partial
			dFQ.clear();

			// Create the difference partial
			const Array<double> x1 = referenceData.constArrayX();
			const Array<double> y1 = referenceData.constArrayY();
			XYData simulatedSQ = weightedSQ.constTotal();

			// Determine allowable range for fit, based on requested values and limits of generated / simulated datasets
			double deltaSQMin = qMin, deltaSQMax = (qMax < 0.0 ? x1.last() : qMax);
			if ((deltaSQMin < x1.first()) || (deltaSQMin < simulatedSQ.xFirst())) deltaSQMin = max(x1.first(), simulatedSQ.xFirst());
			if ((deltaSQMax > x1.last()) || (deltaSQMax > simulatedSQ.xLast())) deltaSQMax = min(x1.last(), simulatedSQ.xLast());

			XYData refSQTrimmed;
			double x;
			for (int n=0; n<x1.nItems(); ++n)
			{
				// Grab experimental data x value
				x = x1.value(n);

				// If this x value is below the minimum Q value we are fitting, set the difference to zero
				if (x < deltaSQMin) dFQ.addPoint(x, 0.0);
				else if (x > deltaSQMax) break;
				else dFQ.addPoint(x, y1.value(n) - simulatedSQ.interpolated(x));
			}

			/*
			 * Fit a function expansion to the differenceFQ
			 */

			// Grab deltaFQ fit storage and make sure its object name is set
			XYData& dFQFit = groupDeltaFQFit[index];
			dFQFit.setObjectName(CharString("%s//DeltaFQFit//%s//%s", uniqueName_.get(), group->name(), module->uniqueName()));

			// Do the coefficient arrays already exist? If so, we re-fit from this set of coefficients. If not, start from scratch
			Array<double>& fitCoefficients = GenericListHelper< Array<double> >::realise(dissolve.processingModuleData(), CharString("FitCoefficients_%s_%s", group->name(), module->uniqueName()), uniqueName_, GenericItem::InRestartFileFlag, &created);

			if (functionType == EPSRModule::GaussianExpansionFunction)
			{
				// Construct our fitting object
				GaussFit coeffMinimiser(dFQ);

				if (created)
				{
					// Start from scratch...
					coeffMinimiser.constructReciprocal(dissolve.pairPotentialRange(), ncoeffp, 	npitss, 0.01, psigma2, 10, 3, 3);
				}
				else
				{
					// TODO Set coefficients and reFit.
				}

				// Store the new fit coefficients
				fitCoefficients = coeffMinimiser.A();

				dFQFit = coeffMinimiser.approximation();
			}
			else if (functionType == EPSRModule::PoissonExpansionFunction)
			{
				// Construct our fitting object
				PoissonFit coeffMinimiser(dFQ);

				if (created)
				{
					// Start from scratch...
					coeffMinimiser.constructReciprocal(dissolve.pairPotentialRange(), ncoeffp, npitss, 0.01, psigma2, psigma2, 10, 3, 3);
				}
				else
				{
					// TODO Set coefficients and reFit.
				}

				// Store the new fit coefficients
				fitCoefficients = coeffMinimiser.C();

				dFQFit = coeffMinimiser.approximation();
			}
			else Messenger::error("Sellotape the developer to the ceiling and poke through with long thin things - he hasn't implemented this expansion function type yet.\n");

			// Increment index
			++index;
		}

		
	}

	
	// Perform actual modification to potential, adding in deltaPhiR calculated over all groups
	if (modifyPotential)
	{
		for (ModuleGroup* group = targetGroups_.first(); group != NULL; group = group->next)
		{
			// Get the delta phi(r) data for this group
			if (!dissolve.processingModuleData().contains(CharString("DeltaPhiR_%s", group->name()), uniqueName())) return Messenger::error("Could not locate delta phi(r) data for group '%s'.\n", group->name());
			const Array2D<XYData>& groupDeltaPhiR = GenericListHelper< Array2D<XYData> >::value(dissolve.processingModuleData(), CharString("DeltaPhiR_%s", group->name()), uniqueName_, Array2D<XYData>());

			i = 0;
			for (AtomType* at1 = dissolve.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++i)
			{
				j = i;
				for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++j)
				{
					// Assess error of partial if requested, and decide whether to adjust potential
					if (onlyWhenErrorStable)
					{
						const XYData& partialErrors = GenericListHelper<XYData>::value(dissolve.processingModuleData(), CharString("PartialError_%s-%s", at1->name(), at2->name()), uniqueName_);
						if (partialErrors.nPoints() >= errorStabilityWindow)
						{
							double yMean;
							double grad = partialErrors.lastGradient(errorStabilityWindow, &yMean);
							double thresholdValue = fabs(0.001*yMean);
							if (fabs(grad) < thresholdValue)
							{
								Messenger::print("Error gradient of last %i points for partial %s-%s is %e %%/step (absolute threshold value is %e, stable = %s) so no potential adjustment will be made.\n", errorStabilityWindow, at1->name(), at2->name(), grad, thresholdValue, DissolveSys::btoa(fabs(grad) <= thresholdValue));
								continue;
							}
						}
					}

					// Grab pointer to the relevant pair potential
					PairPotential* pp = dissolve.pairPotential(at1, at2);
					if (!pp)
					{
						Messenger::error("Failed to find PairPotential for AtomTypes '%s' and '%s'.\n", at1->name(), at2->name());
						return false;
					}

					pp->adjustUAdditional(groupDeltaPhiR.value(i,j));
				}
			}
		}
	}

	/*
	 * Calculate overall phi magnitude and clamp individual magnitudes if required
	 */
	double phiMagTot = 0.0;
	i = 0;
	for (AtomType* at1 = dissolve.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++i)
	{
		j = i;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++j)
		{
			// Grab pointer to the relevant pair potential
			PairPotential* pp = dissolve.pairPotential(at1, at2);
			if (!pp) continue;

			// Calculate phi magnitude for this pair potential
			double phiMag = pp->uAdditional().absIntegral();

// 			// Clamp it?  TODO
// 			if (modifyPotential && (ereq > 0.0) && (phiMag > phiMax))
// 			{
// 				double factor = phiMax / phiMag;
// 				pp->uAdditional().arrayY() *= factor;
// 				phiMag = phiMax;
// 			}

			// Sum into phiMagTot
			phiMagTot += phiMag;
		}
	}

	Messenger::print("Current magnitude of additional phi(r) over all pair potentials is %12.4e kJ/mol/Angstrom.\n", phiMagTot);

	// Realise the phiMag array and make sure its object name is set
	XYData& phiArray = GenericListHelper<XYData>::realise(dissolve.processingModuleData(), "PhiMag", uniqueName_, GenericItem::InRestartFileFlag);
	phiArray.setObjectName(CharString("%s//PhiMag", uniqueName_.get()));
	phiArray.addPoint(dissolve.iteration(), phiMagTot);

	return true;
}

