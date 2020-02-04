/*
	*** Refine Module - Processing
	*** src/modules/refine/process.cpp
	Copyright T. Youngs 2012-2020

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

#include "modules/refine/refine.h"
#include "main/dissolve.h"
#include "modules/energy/energy.h"
#include "modules/rdf/rdf.h"
#include "math/error.h"
#include "math/filters.h"
#include "math/ft.h"
#include "math/gaussfit.h"
#include "math/integrator.h"
#include "math/regression.h"
#include "module/group.h"
#include "classes/scatteringmatrix.h"
#include "classes/weights.h"
#include "classes/atomtype.h"
#include "classes/partialset.h"
#include "data/isotopes.h"
#include "base/sysfunc.h"
#include "genericitems/listhelper.h"

// Run main processing
bool RefineModule::process(Dissolve& dissolve, ProcessPool& procPool)
{
	int i, j;
	bool found;

	/*
	 * Get Keyword Options
	 */
	const RefineModule::MatrixAugmentationStyle augmentationStyle = keywords_.enumeration<RefineModule::MatrixAugmentationStyle>("Augmentation");
	const double augmentationParam = keywords_.asDouble("AugmentationParam");
	const bool autoMinimumRadii = keywords_.asBool("AutoMinimumRadius");
	const bool smoothPhiR = keywords_.asBool("DeltaPhiRSmoothing");
	const int phiRSmoothK = keywords_.asInt("DeltaPhiRSmoothK");
	const int phiRSmoothM = keywords_.asInt("DeltaPhiRSmoothM");
	const double errorStabilityThreshold = keywords_.asDouble("ErrorStabilityThreshold");
	const int errorStabilityWindow = keywords_.asInt("ErrorStabilityWindow");
	//const double gaussianAccuracy = keywords_.asDouble("GaussianAccuracy");
	const RefineModule::PotentialInversionMethod inversionMethod = keywords_.enumeration<RefineModule::PotentialInversionMethod>("InversionMethod");
	const double globalMinimumRadius = keywords_.asDouble("MinimumRadius");
	const double globalMaximumRadius = keywords_.asDouble("MaximumRadius");
// 	const bool modifyBonds = keywords_.asBool("ModifyBonds");
	const bool modifyPotential = keywords_.asBool("ModifyPotential");
	const bool onlyWhenEnergyStable = keywords_.asBool("OnlyWhenEnergyStable");
	const bool onlyWhenErrorStable = keywords_.asBool("OnlyWhenErrorStable");
	const double phiMax = keywords_.asDouble("PhiMax");
	const double qMax = keywords_.asDouble("QMax");
	const double qMin = keywords_.asDouble("QMin");
	const double truncationWidth = keywords_.asDouble("TruncationWidth");
	const WindowFunction& windowFunction = keywords_.retrieve<WindowFunction>("WindowFunction", WindowFunction());

	// Print option summary
	if (augmentationStyle == RefineModule::NoAugmentation) Messenger::print("Refine: No augmentation of scattering matrix will be performed.\n");
	else Messenger::print("Refine: Augmentation of scattering matrix will be performed (style = '%s', parameter = %e).\n", matrixAugmentationStyles().keyword(augmentationStyle), augmentationParam);
	if (autoMinimumRadii) Messenger::print("Refine: Minimum radii of generated potentials will be determined automatically (min/max = %f/%f Angstroms).\n", globalMinimumRadius, globalMaximumRadius);
	else Messenger::print("Refine: Minimum radius of %f Angstroms will be applied to all generated potentials.\n", globalMinimumRadius);
	if (smoothPhiR) Messenger::print("Refine: Generated phi(r) will be smoothed using KZ filter (k = %i, m = %i).\n", phiRSmoothK, phiRSmoothM);
// 	if (modifyBonds) Messenger::print("Refine: Equilibrium master bond distances will be modified.\n");
	if (modifyPotential) Messenger::print("Refine: Perturbations to interatomic potentials will be generated and applied.\n");
	else Messenger::print("Refine: Perturbations to interatomic potentials will be generated only (current potentials will not be modified).\n");
	if (onlyWhenEnergyStable) Messenger::print("Refine: Potential refinement will only be performed if all related Configuration energies are stable.\n");
	if (onlyWhenErrorStable) Messenger::print("Refine: Potential refinement will only be performed if all percentage errors with reference data are stable.\n");
	if (phiMax >= 0) Messenger::print("Refine: Limit of additional potential for any one pair potential is %f kJ/mol/Angstrom.\n", phiMax);
	else Messenger::warn("Refine: No limits will be applied to the magnitudes of additional potentials.\n");
	Messenger::print("Refine: Potential inversion method to employ is '%s'.\n", potentialInversionMethods().keyword(inversionMethod));
	Messenger::print("Refine: Range for potential generation is %f < Q < %f Angstroms**-1.\n", qMin, qMax);
	if (windowFunction.function() == WindowFunction::NoWindow) Messenger::print("Refine: No window function will be applied in Fourier transforms of S(Q) to g(r).\n");
	else Messenger::print("Refine: Window function to be applied in Fourier transforms of S(Q) to g(r) is %s (%s).", WindowFunction::functionType(windowFunction.function()), windowFunction.parameterSummary().get());
	Messenger::print("\n");


	/*
	 * Do we have targets to refine against?
	 */
	if (groupedTargets_.nModules() == 0) return Messenger::error("At least one Module target containing suitable data must be provided.\n");


	/*
	 * Make a list of all Configurations related to all targets
	 */
	RefList<Configuration> configs;
	RefDataListIterator<Module,ModuleGroup*> allTargetsIterator(groupedTargets_.modules());
	while (Module* module = allTargetsIterator.iterate())
	{
		for (Configuration* cfg : module->targetConfigurations()) configs.addUnique(cfg);
	}
	Messenger::print("%i Configuration(s) are involved over all target data.\n", configs.nItems());


	/*
	 * Calculate current percentage errors in calculated vs reference target data
	 */
	int nUnstableData = 0;
	ListIterator<ModuleGroup> groupIterator(groupedTargets_.groups());
	while (ModuleGroup* group = groupIterator.iterate())
	{
		// Grab Module list for this group and set up an iterator
		for (Module* module : group->modules())
		{
			// Realise the error array and make sure its object name is set
			Data1D& errors = GenericListHelper<Data1D>::realise(dissolve.processingModuleData(), CharString("Error_%s", module->uniqueName()), uniqueName_, GenericItem::InRestartFileFlag);
			errors.setObjectTag(CharString("%s//Error//%s", uniqueName_.get(), module->uniqueName()));

			// Calculate our error based on the type of Module
			double error = 100.0;
			if (DissolveSys::sameString(module->type(), "NeutronSQ"))
			{
				// Retrieve the ReferenceData from the Module (as Data1D)
				const Data1D& referenceData = GenericListHelper<Data1D>::value(dissolve.processingModuleData(), "ReferenceData", module->uniqueName(), Data1D(), &found);
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
				Data1D calcSQTotal = calcSQ.constTotal();

				error = Error::percent(referenceData, calcSQTotal);

				// Calculate difference
				Data1D& differenceData = GenericListHelper<Data1D>::realise(dissolve.processingModuleData(), CharString("DifferenceData_%s", module->uniqueName()), uniqueName());
				differenceData.setObjectTag(CharString("%s//Difference//%s", uniqueName_.get(), module->uniqueName()));
				differenceData = referenceData;
				Interpolator::addInterpolated(differenceData, calcSQTotal, -1.0);
			}
			else return Messenger::error("Unrecognised Module type '%s', so can't calculate error.", module->type());

			// Store the percentage error
			errors.addPoint(dissolve.iteration(), error);
			Messenger::print("Current error for reference data '%s' is %f%%.\n", module->uniqueName(), error);

			// Assess the stability of the current error
			if (errorStabilityWindow > errors.nValues()) ++nUnstableData;
			else
			{
				double yMean;
				double grad = Regression::linear(errors, errorStabilityWindow, yMean);
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
	groupIterator.restart();
	while (ModuleGroup* group = groupIterator.iterate())
	{
		Messenger::print("Generating dPhiR from target group '%s'...\n", group->name());

		// Grab Module list for this group
		const RefList<Module>& targetModules = group->modules();

		/*
		 * Update our full scattering matrix, and use it to generate partials from the supplied reference data
		 */

		// Create temporary storage for our summed UnweightedSQ, and related quantities such as density and sum factors
		Array2D<Data1D>& combinedUnweightedSQ = GenericListHelper< Array2D<Data1D> >::realise(dissolve.processingModuleData(), "UnweightedSQ", uniqueName_, GenericItem::InRestartFileFlag);
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
		for (AtomType* at1 = dissolve.atomTypes().first(); at1 != NULL; at1 = at1->next(), ++i)
		{
			j = i;
			for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next(), ++j) combinedUnweightedSQ.at(i,j).setObjectTag(CharString("%s//UnweightedSQ//%s//%s-%s", uniqueName(), group->name(), at1->name(), at2->name()));
		}

		// Realise storage for generated S(Q), and reinitialise the scattering matrix
		Array2D<Data1D>&  estimatedSQ = GenericListHelper< Array2D<Data1D> >::realise(dissolve.processingModuleData(), "EstimatedSQ", uniqueName_, GenericItem::InRestartFileFlag);
		scatteringMatrix_.initialise(dissolve.atomTypes(), estimatedSQ, uniqueName_, group->name());

		// Get factor with which to add data, based on the requested AugmentationStyle
		double dataFactor = 1.0;
		if (augmentationStyle == RefineModule::PartialsAugmentation) dataFactor = augmentationParam;

		// Add a row in the scattering matrix for each target in the group
		for (Module* module : targetModules)
		{
			// Retrieve the reference data and associated Weights matrix and source unweighted partials
			const Data1D& referenceData = GenericListHelper<Data1D>::value(dissolve.processingModuleData(), "ReferenceData", module->uniqueName(), Data1D(), &found);
			if (!found) return Messenger::error("Could not locate ReferenceData for target '%s'.\n", module->uniqueName());
			Weights& weights = GenericListHelper<Weights>::retrieve(dissolve.processingModuleData(), "FullWeights", module->uniqueName(), Weights(), &found);
			if (!found) return Messenger::error("Could not locate Weights for target '%s'.\n", module->uniqueName());
			const PartialSet& unweightedSQ = GenericListHelper<PartialSet>::value(dissolve.processingModuleData(), "UnweightedSQ", module->uniqueName(), PartialSet(), &found);
			if (!found) return Messenger::error("Could not locate UnweightedSQ for target '%s'.\n", module->uniqueName());
			double rho = GenericListHelper<
			double>::value(dissolve.processingModuleData(), "EffectiveRho", module->uniqueName(), 0.0, &found);
			if (!found) return Messenger::error("Could not locate EffectiveRho for target '%s'.\n", module->uniqueName());

			// Add a row to our scattering matrix
			if (!scatteringMatrix_.addReferenceData(referenceData, weights, dataFactor)) return Messenger::error("Failed to add target data '%s' to weights matrix.\n", module->uniqueName());

			// Sum up the unweighted partials and density from this target
			double factor = 1.0;
			i = 0;
			for (AtomTypeData* atd1 = unweightedSQ.atomTypes().first(); atd1 != NULL; atd1 = atd1->next(), ++i)
			{
				j = i;
				for (AtomTypeData* atd2 = atd1; atd2 != NULL; atd2 = atd2->next(), ++j)
				{
					double globalI = atd1->atomType()->index();
					double globalJ = atd2->atomType()->index();

					Data1D partialIJ = unweightedSQ.constPartial(i,j);
					Interpolator::addInterpolated(combinedUnweightedSQ.at(globalI, globalJ), partialIJ, factor);
					combinedRho.at(globalI, globalJ) += rho * factor;
					combinedFactor.at(globalI, globalJ) += factor;
					combinedCWeights.at(globalI, globalJ) += weights.concentrationProduct(i,j);
				}
			}
		}

		// Normalise the combined values
		for (i=0; i<dissolve.nAtomTypes(); ++i)
		{
			for (j=i; j<dissolve.nAtomTypes(); ++j)
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

		if (augmentationStyle == RefineModule::PartialsAugmentation)
		{
			// Create the array
			simulatedReferenceData_.createEmpty(combinedUnweightedSQ.linearArraySize());
			i = 0;
			for (AtomType* at1 = dissolve.atomTypes().first(); at1 != NULL; at1 = at1->next(), ++i)
			{
				j = i;
				for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next(), ++j)
				{
					// Weight in the matrix will be based on the natural isotope and the summed concentration weight
					double factor = Isotopes::naturalIsotope(at1->element())->boundCoherent() * Isotopes::naturalIsotope(at2->element())->boundCoherent() * 0.01;
					factor *= combinedCWeights.constAt(i,j);

					// Copy the unweighted data and wight weight it according to the natural isotope / concentration factor calculated above
					Data1D data = combinedUnweightedSQ.at(i, j);
					data.values() *= factor;
					data.setName(CharString("Simulated %s-%s", at1->name(), at2->name()));

					// Add this partial data to the scattering matrix
					if (!scatteringMatrix_.addPartialReferenceData(data, at1, at2, factor, (1.0 - augmentationParam))) return Messenger::error("Refine: Failed to augment scattering matrix with partial %s-%s.\n", at1->name(), at2->name());
				}
			}
		}
		else if (augmentationStyle != RefineModule::NoAugmentation)
		{
			Messenger::error("Slap the developer - he hasn't implemented this augmentation option yet.\n");
			return false;
		}

		// If the matrix is underdetermined, must raise an error here
		if (scatteringMatrix_.underDetermined()) return Messenger::error("Can't proceed with an underdetermined scattering matrix. Select an augmentation style other than 'None'.\n");

		scatteringMatrix_.finalise();
		scatteringMatrix_.print();

		/*
		 * Generate S(Q) from completed scattering matrix
		 */

		// Generate the scattering matrix 
		scatteringMatrix_.generatePartials(estimatedSQ);

		/*
		 * Calculate g(r) from estimatedSQ
		 */

		Array2D<Data1D>& estimatedGR = GenericListHelper< Array2D<Data1D> >::realise(dissolve.processingModuleData(), "EstimatedGR", uniqueName_, GenericItem::InRestartFileFlag);
		estimatedGR.initialise(dissolve.nAtomTypes(), dissolve.nAtomTypes(), true);
		i = 0;
		for (AtomType* at1 = dissolve.atomTypes().first(); at1 != NULL; at1 = at1->next(), ++i)
		{
			j = i;
			for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next(), ++j)
			{
				// Grab experimental g(r) contained and make sure its object name is set
				Data1D& expGR = estimatedGR.at(i,j);
				expGR.setObjectTag(CharString("%s//EstimatedGR//%s//%s-%s", uniqueName_.get(), group->name(), at1->name(), at2->name()));

				// Copy experimental S(Q) and FT it
				expGR = estimatedSQ.at(i,j);
				Fourier::sineFT(expGR, 1.0 / (2 * PI * PI * combinedRho.at(i,j)), 0.0, 0.05, 30.0, WindowFunction(WindowFunction::Lorch0Window));
				expGR.values() += 1.0;
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
		Array2D<Data1D>& deltaSQ = GenericListHelper< Array2D<Data1D> >::realise(dissolve.processingModuleData(), "DeltaSQ", uniqueName_, GenericItem::InRestartFileFlag, &created);
		if (created) deltaSQ.initialise(nTypes, nTypes, true);

		i = 0;
		for (AtomType* at1 = dissolve.atomTypes().first(); at1 != NULL; at1 = at1->next(), ++i)
		{
			j = i;
			for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next(), ++j)
			{
				// Grab difference partial and make sure its object name is set
				Data1D& dSQ = deltaSQ.at(i, j);
				dSQ.setObjectTag(CharString("%s//DeltaSQ//%s//%s-%s", uniqueName_.get(), group->name(), at1->name(), at2->name()));

				// Reset the difference partial
				dSQ.clear();

				// Create the difference partial
				const Array<double> x1 = estimatedSQ.at(i, j).constXAxis();
				const Array<double> y1 = estimatedSQ.at(i, j).constValues();
				Data1D& simulatedSQ = combinedUnweightedSQ.at(i,j);
				Interpolator interpolatedSimSQ(simulatedSQ);

				// Determine allowable range for fit, based on requested values and limits of generated / simulated datasets
				double deltaSQMin = qMin, deltaSQMax = (qMax < 0.0 ? x1.lastValue() : qMax);
				if ((deltaSQMin < x1.firstValue()) || (deltaSQMin < simulatedSQ.xAxis().firstValue())) deltaSQMin = max(x1.firstValue(), simulatedSQ.xAxis().firstValue());
				if ((deltaSQMax > x1.lastValue()) || (deltaSQMax > simulatedSQ.xAxis().lastValue())) deltaSQMax = min(x1.lastValue(), simulatedSQ.xAxis().lastValue());

				Data1D refSQTrimmed;
				double x;
				for (int n=0; n<x1.nItems(); ++n)
				{
					x = x1.constAt(n);
					if (x < deltaSQMin) continue;
					if (x > deltaSQMax) break;
					refSQTrimmed.addPoint(x, y1.constAt(n));

					dSQ.addPoint(x, y1.constAt(n) - interpolatedSimSQ.y(x));
				}

				// Calculate current error between experimental and simulation partials and sum it into our array
				globalCombinedErrors.at(i, j) += Error::percent(refSQTrimmed, simulatedSQ);
			}
		}

		/*
		 * Create perturbations to interatomic potentials
		 */
		const double weighting = keywords_.asDouble("Weighting");
		Array2D<Data1D>& groupDeltaPhiR = GenericListHelper< Array2D<Data1D> >::realise(dissolve.processingModuleData(), CharString("DeltaPhiR_%s", group->name()), uniqueName_, GenericItem::InRestartFileFlag, &created);
		if (created) groupDeltaPhiR.initialise(nTypes, nTypes, true);
		Array2D<Data1D>& groupDeltaGR = GenericListHelper< Array2D<Data1D> >::realise(dissolve.processingModuleData(), CharString("DeltaGR_%s", group->name()), uniqueName_, GenericItem::InRestartFileFlag, &created);
		if (created) groupDeltaGR.initialise(nTypes, nTypes, true);
	// 	if (modifyBonds)
	// 	{
	// 		Array2D<Data1D>& deltaGRBond = GenericListHelper< Array2D<Data1D> >::realise(dissolve.processingModuleData(), "DeltaGRBond", uniqueName_, GenericItem::InRestartFileFlag, &created);
	// 		if (created) deltaGRBond.initialise(nTypes, nTypes, true);
	// 	}

		// Set up / updated minimum radii array, considering all involved Configurations
		Array2D<double>& minimumRadii = GenericListHelper< Array2D<double> >::realise(dissolve.processingModuleData(), "MinimumRadii", uniqueName_, GenericItem::InRestartFileFlag, &created);
		if (created) minimumRadii.initialise(nTypes, nTypes, true);
		if (autoMinimumRadii)
		{
			// Define a fraction of the determined g(r) non-zero point that will become our radius limit
			const double rFraction = 0.95;
			const double thresholdValue = 0.1;
			i = 0;
			for (AtomType* at1 = dissolve.atomTypes().first(); at1 != NULL; at1 = at1->next(), ++i)
			{
				j = i;
				for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next(), ++j)
				{
					// Grab unbound g(r)
					Data1D& gr = summedUnweightedGR.unboundPartial(i, j);

					// Find first non-zero (above the threshold value) point in g(r)
					int n;
					for (n=0; n<gr.nValues(); ++n) if (gr.value(n) > thresholdValue) break;
					if (n < gr.nValues())
					{
						double x = gr.xAxis(n) * rFraction;
						if (x < globalMinimumRadius) minimumRadii.at(i,j) = globalMinimumRadius;
						else if (x < globalMaximumRadius) minimumRadii.at(i,j) = x;
						else minimumRadii.at(i,j) = globalMaximumRadius;
						
					}
					else minimumRadii.at(i,j) = globalMaximumRadius;

					Messenger::print("Minimum radius for %s-%s interatomic potential determined to be %f Angstroms.\n", at1->name(), at2->name(), minimumRadii.constAt(i,j));
				}
			}
		}
		else minimumRadii = globalMinimumRadius;
		
		const double ppRange = dissolve.pairPotentialRange();
		const double ppDelta = dissolve.pairPotentialDelta();
		double weight;
		Data1D cr;
		Array<double> crgr;
		i = 0;
		for (AtomType* at1 = dissolve.atomTypes().first(); at1 != NULL; at1 = at1->next(), ++i)
		{
			j = i;
			for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next(), ++j)
			{
				// Grab potential perturbation container, clear it, and make sure its object name is set
				Data1D& dPhiR = groupDeltaPhiR.at(i, j);
				dPhiR.clear();
				dPhiR.setObjectTag(CharString("%s//DeltaPhiR//%s//%s-%s", uniqueName_.get(), group->name(), at1->name(), at2->name()));

				// Grab delta g(r) container and make sure its object name is set
				Data1D& inversion = groupDeltaGR.at(i, j);
				inversion.setObjectTag(CharString("%s//Inversion//%s//%s-%s", uniqueName_.get(), group->name(), at1->name(), at2->name()));

				// Set the default weighting factor for the pair potential addition
				weight = weighting;

	// 			// If we are modifying bond equilibrium distances, search for signatures in the deltaGR, do this now.
	// 			// Such signatures will be removed from the final dGR, and will thus not contribute to the PairPotential addition.
	// 			if (modifyBonds)
	// 			{
	// 				// Grab the deltaGRBond container and make sure its object name is set
	// 				Array2D<Data1D>& deltaGRBonds = GenericListHelper< Array2D<Data1D> >::retrieve(dissolve.processingModuleData(), "DeltaGRBond", uniqueName_);
	// 				Data1D& deltaGRBond = deltaGRBonds.ref(i,j);
	// 				deltaGRBond.setObjectTag(CharString("%s//DeltaGRBond//%s-%s", uniqueName_.get(), at1->name(), at2->name()));
	// 
	// 				if (!modifyBondTerms(dissolve, dGR, at1, at2, deltaGRBond)) return false;
	// 				dGR.addInterpolated(deltaGRBond, -1.0);
	// 			}

				// Create a perturbation
				if (inversionMethod == RefineModule::DirectFourierPotentialInversion)
				{
					// Copy the delta S(Q) and do the inverse FT to get the delta [g(r) - 1]
					inversion = deltaSQ.at(i, j);
					Fourier::sineFT(inversion, 1.0 / (2 * PI * PI * combinedRho.at(i,j)), ppDelta, ppDelta, ppRange, windowFunction);

					dPhiR = inversion;
					dPhiR.values() *= -1.0;
				}
				else if (inversionMethod == RefineModule::DirectGaussianPotentialInversion)
				{
					// Perform a Gaussian fit and do the inverse FT to get the delta [g(r) - 1]
					GaussFit gaussFit(deltaSQ.at(i, j));
					double error = 999.999; //gaussFit.constructReciprocal();
					Messenger::print("Fitted function has error of %f%% with original delta S(Q) (nGaussians = %i).\n", error, gaussFit.nGaussians());

					// Store fitted parameters
					GenericListHelper< Array<double> >::realise(dissolve.processingModuleData(), CharString("%s-%s-GaussianX", at1->name(), at2->name()), uniqueName_, GenericItem::InRestartFileFlag) = gaussFit.x();
					GenericListHelper< Array<double> >::realise(dissolve.processingModuleData(), CharString("%s-%s-GaussianA", at1->name(), at2->name()), uniqueName_, GenericItem::InRestartFileFlag) = gaussFit.A();
					GenericListHelper< Array<double> >::realise(dissolve.processingModuleData(), CharString("%s-%s-GaussianFWHM", at1->name(), at2->name()), uniqueName_, GenericItem::InRestartFileFlag) = gaussFit.fwhm();

					// DEBUG
// 					if (false)
// 					{
// 						deltaSQ.at(i, j).save(CharString("%s-%s.orig", at1->name(), at2->name()));
// 						gaussFit.approximation().save(CharString("%s-%s.approx", at1->name(), at2->name()));
// 						gaussFit.saveCoefficients(CharString("%s-%s.coeff", at1->name(), at2->name()));
// // 						gaussFit.saveFTGaussians(CharString("%s-%s", at1->name(), at2->name()), 0.01);
// 						gaussFit.approximation(FunctionSpace::RealSpace, 1.0 / (2 * PI * PI * combinedRho.at(i,j)), ppDelta, ppDelta, ppRange).save(CharString("%s-%s.ft", at1->name(), at2->name()));
// 					}

					// Fourier transform the approximation, and store this as our inversion
					inversion = gaussFit.approximation(FunctionSpace::RealSpace, 1.0 / (2 * PI * PI * combinedRho.at(i,j)), ppDelta, ppDelta, ppRange);
					dPhiR = inversion;
					dPhiR.values() *= -1.0;
				}
				else if (inversionMethod == RefineModule::PercusYevickPotentialInversion)
				{
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
					cr = calculateCR(deltaSQ.at(i, j), 1.0 / (2.0*PI*PI*combinedRho.at(i,j)), ppDelta, ppDelta, ppRange, windowFunction);

					// inversion contains the FT of the delta S(Q), and oscillates around zero.
					// Scale the data to have a maximum deviation from zero of 1.0, so that we avoid any possibility to get NaNs when taking the ln later on

					/*
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
					*/
				}
				else if (inversionMethod == RefineModule::HypernettedChainPotentialInversion)
				{
					/*
					 * The original hypernetted chain closure relationship for solving the Ornstein-Zernike equation is:
					 * 
					 *  phi(r) = g(r) - c(r) - 1.0 - ln(g(r))
					 * 
					 * As with the Percus-Yevick closure above, we must adjust the relationship slightly...
					 */

					/*
					// Calculate c(r) from the delta S(Q)
					cr = calculateCR(deltaSQ.ref(i, j), 1.0 / (2.0*PI*PI*rho), ppDelta, ppDelta, ppRange, windowFunction, BroadeningFunction(), true);

					scaleFactor = dGR.arrayY().maxAbs() * 0.75;
					if (scaleFactor < 1.0) scaleFactor = 1.0;
					dGR.arrayY() /= scaleFactor;

					cr.arrayY() /= scaleFactor;

					for (int n=0; n<dGR.nPoints(); ++n) dPhiR.addPoint(dGR.x(n), dGR.y(n) - cr.y(n) - 1.0 - log(dGR.y(n) + 1.0));
					*/
				}
				else return Messenger::error("Potential inversion method %i not accounted for.\n", inversionMethod);

				// Apply smooth zeroing of potential up to the minimum distance, and truncate at the end
				double minimumRadius = minimumRadii.constAt(i,j);
				const double truncationStart = minimumRadius - truncationWidth;
				double r;
				Array<double>& y = dPhiR.values();
				for (int n=0; n<dPhiR.nValues(); ++n)
				{
					r = dPhiR.xAxis(n);
					if (r < truncationStart) y[n] = 0.0;
					else if (r > minimumRadius) break;
					else y[n] *= 0.5 - 0.5*cos(PI*0.5*(r-truncationStart)/(truncationWidth*0.5));
				}

				// Smooth phi(r)?
				if (smoothPhiR) Filters::kolmogorovZurbenko(dPhiR, phiRSmoothK, phiRSmoothM);

				// Make sure we go smoothly to zero at the limit of the potential
				for (int n=0; n<dPhiR.nValues(); ++n) dPhiR.value(n) *= 1.0 - double(n)/(dPhiR.nValues()-1);

				// Apply factor to additional potential
				dPhiR.values() *= weight;
			}
		}
	}

	/*
	 * Normalise and store our combined partial errors
	 */
	i = 0;
	for (AtomType* at1 = dissolve.atomTypes().first(); at1 != NULL; at1 = at1->next(), ++i)
	{
		j = i;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next(), ++j)
		{
			Data1D& partialErrors = GenericListHelper<Data1D>::realise(dissolve.processingModuleData(), CharString("PartialError_%s-%s", at1->name(), at2->name()), uniqueName_, GenericItem::InRestartFileFlag);
			// TODO This will be a straight sum of errors over groups, and may not be entirely representative? Needs to be weighted?
			partialErrors.addPoint(dissolve.iteration(), globalCombinedErrors.constAt(i,j));
		}
	}

	// Perform actual modification to potential, adding in deltaPhiR calculated over all groups
	if (modifyPotential)
	{
		groupIterator.restart();
		while (ModuleGroup* group = groupIterator.iterate())
		{
			// Get the delta phi(r) data for this group
			if (!dissolve.processingModuleData().contains(CharString("DeltaPhiR_%s", group->name()), uniqueName())) return Messenger::error("Could not locate delta phi(r) data for group '%s'.\n", group->name());
			const Array2D<Data1D>& groupDeltaPhiR = GenericListHelper< Array2D<Data1D> >::value(dissolve.processingModuleData(), CharString("DeltaPhiR_%s", group->name()), uniqueName_, Array2D<Data1D>());

			i = 0;
			for (AtomType* at1 = dissolve.atomTypes().first(); at1 != NULL; at1 = at1->next(), ++i)
			{
				j = i;
				for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next(), ++j)
				{
					// Assess error of partial if requested, and decide whether to adjust potential
					if (onlyWhenErrorStable)
					{
						const Data1D& partialErrors = GenericListHelper<Data1D>::value(dissolve.processingModuleData(), CharString("PartialError_%s-%s", at1->name(), at2->name()), uniqueName_);
						if (partialErrors.nValues() >= errorStabilityWindow)
						{
							double yMean;
							double grad = Regression::linear(partialErrors, errorStabilityWindow, yMean);
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

					pp->adjustUAdditional(groupDeltaPhiR.constAt(i,j));
				}
			}
		}
	}

	/*
	 * Calculate overall phi magnitude and clamp individual magnitudes if required
	 */
	double phiMagTot = 0.0;
	i = 0;
	for (AtomType* at1 = dissolve.atomTypes().first(); at1 != NULL; at1 = at1->next(), ++i)
	{
		j = i;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next(), ++j)
		{
			// Grab pointer to the relevant pair potential
			PairPotential* pp = dissolve.pairPotential(at1, at2);
			if (!pp) continue;

			// Calculate phi magnitude for this pair potential
			double phiMag = Integrator::absTrapezoid(pp->uAdditional());

			// Clamp it?
			if (modifyPotential && (phiMax > 0.0) && (phiMag > phiMax))
			{
				double factor = phiMax / phiMag;
				pp->uAdditional().values() *= factor;
				phiMag = phiMax;
			}

			// Sum into phiMagTot
			phiMagTot += phiMag;
		}
	}

	Messenger::print("Current magnitude of additional phi(r) over all pair potentials is %12.4e kJ/mol/Angstrom.\n", phiMagTot);

	// Realise the phiMag array and make sure its object name is set
	Data1D& phiArray = GenericListHelper<Data1D>::realise(dissolve.processingModuleData(), "PhiMag", uniqueName_, GenericItem::InRestartFileFlag);
	phiArray.setObjectTag(CharString("%s//PhiMag", uniqueName_.get()));
	phiArray.addPoint(dissolve.iteration(), phiMagTot);

	return true;
}

