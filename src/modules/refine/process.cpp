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
#include "modules/rdf/rdf.h"
#include "math/gaussfit.h"
#include "classes/scatteringmatrix.h"
#include "classes/weights.h"
#include "classes/atomtype.h"
#include "classes/partialset.h"
#include "base/sysfunc.h"
#include "templates/genericlisthelper.h"

// Return whether the Module has a processing stage
bool RefineModule::hasProcessing()
{
	return true;
}

// Run main processing
bool RefineModule::process(DUQ& duq, ProcessPool& procPool)
{
	int i, j;
	bool found;

	/*
	 * Set Module data target
	 */
	GenericList& moduleData = configurationLocal_ ? targetConfigurations_.firstItem()->moduleData() : duq.processingModuleData();

	/*
	 * Get Keyword Options
	 */
	const RefineModule::MatrixAugmentationStyle augmentationStyle = RefineModule::matrixAugmentationStyle(keywords_.asString("Augmentation"));
	const double augmentationParam = keywords_.asDouble("AugmentationParam");
	const bool autoMinimumRadii = keywords_.asBool("AutoMinimumRadius");
	const bool smoothPhiR = keywords_.asBool("DeltaPhiRSmoothing");
	const int phiRSmoothK = keywords_.asDouble("DeltaPhiRSmoothK");
	const int phiRSmoothM = keywords_.asDouble("DeltaPhiRSmoothM");
	const double gaussianAccuracy = keywords_.asDouble("GaussianAccuracy");
	const RefineModule::PotentialInversionMethod inversionMethod = RefineModule::potentialInversionMethod(keywords_.asString("InversionMethod"));
	const double globalMinimumRadius = keywords_.asDouble("MinimumRadius");
	const double globalMaximumRadius = keywords_.asDouble("MaximumRadius");
// 	const bool modifyBonds = keywords_.asBool("ModifyBonds");
	const bool modifyPotential = keywords_.asBool("ModifyPotential");
	bool onlyWhenStable = keywords_.asBool("OnlyWhenStable");
	double phiLimit = keywords_.asDouble("PhiLimit");
	const double truncationWidth = keywords_.asDouble("TruncationWidth");
	const WindowFunction& windowFunction = KeywordListHelper<WindowFunction>::retrieve(keywords_, "WindowFunction", WindowFunction());

	// Print option summary
	if (augmentationStyle == RefineModule::NoAugmentation) Messenger::print("Refine: No augmentation of scattering matrix will be performed.\n");
	else Messenger::print("Refine: Augmentation of scattering matrix will be performed (style = '%s', parameter = %e).\n", RefineModule::matrixAugmentationStyle(augmentationStyle), augmentationParam);
	if (autoMinimumRadii) Messenger::print("Refine: Minimum radii of generated potentials will be determined automatically (min/max = %f/%f Angstroms).\n", globalMinimumRadius, globalMaximumRadius);
	else Messenger::print("Refine: Minimum radius of %f Angstroms will be applied to all generated potentials.\n", globalMinimumRadius);
	if (smoothPhiR) Messenger::print("Refine: Generated phi(r) will be smoothed using KZ filter (k = %i, m = %i).\n", phiRSmoothK, phiRSmoothM);
// 	if (modifyBonds) Messenger::print("Refine: Equilibrium master bond distances will be modified.\n");
	if (modifyPotential) Messenger::print("Refine: Perturbations to interatomic potentials will be generated and applied.\n");
	else Messenger::print("Refine: Perturbations to interatomic potentials will be generated only (current potentials will not be modified).\n");
	if (onlyWhenStable) Messenger::print("Refine: Potential refinement will only be attempted if all related Configuration energies are stable.\n");
	if (phiLimit >= 0) Messenger::print("Refine: Limit of additional potential phi(r) across all potentials is %f kJ/mol/Angstrom.\n", phiLimit);
	else Messenger::warn("Refine: No limits will be applied to additional potential magnitude.\n");
	if (windowFunction.function() == WindowFunction::NoWindow) Messenger::print("Refine: No window function will be applied in Fourier transforms of S(Q) to g(r).\n");
	else Messenger::print("Refine: Window function to be applied in Fourier transforms of S(Q) to g(r) is %s (%s).", WindowFunction::functionType(windowFunction.function()), windowFunction.parameterSummary().get());

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
	 * Are the energies of all involved Configurations stable (if OnlyWhenStable option is on)
	 */
	if (onlyWhenStable)
	{
		bool anyFailed = false;

		RefListIterator<Configuration,bool> configIterator(configs);
		while (Configuration* cfg = configIterator.iterate())
		{
			// First, check that the EnergyStable module data exists
			if (cfg->moduleData().contains("EnergyStable"))
			{
				bool stable = GenericListHelper<bool>::retrieve(cfg->moduleData(), "EnergyStable", "");
				if (!stable)
				{
					Messenger::print("Energy for Configuration '%s' is not yet stable. No potential refinement will be performed this iteration.\n", cfg->name());
					anyFailed = true;
				}
			}
			else
			{
				Messenger::error("No energy stability information found in Configuration '%s' - check your setup.\n", cfg->name());
				return false;
			}
		}

		// If one or more Configurations failed the energy check, we can't go any further, so exit now
		if (anyFailed) return true;
	}


	/*
	 * Loop over groups of defined Module targets.
	 * We will generate a contribution to dPhiR from each and blend them together.
	 */
	for (ModuleGroup* group = targetGroups_.first(); group != NULL; group = group->next)
	{
		Messenger::print("Generating dPhiR from target group '%s'...\n", group->name());

		// Grab Module list for this group and set up an iterator
		const RefList<Module,bool>& targetModules = group->modules();
		RefListIterator<Module,bool> targetIterator(targetModules);

		/*
		 * Calculate current percentage errors in calculated vs reference target data
		 */
		targetIterator.restart();
		while (Module* module = targetIterator.iterate())
		{
			// Realise the error array and make sure its object name is set
			XYData& errors = GenericListHelper<XYData>::realise(duq.processingModuleData(), CharString("Error_%s", module->uniqueName()), uniqueName_, GenericItem::InRestartFileFlag);
			errors.setObjectName(CharString("%s//Error//%s", uniqueName_.get(), module->uniqueName()));

			// Calculate our error based on the type of Module
			double error = 100.0;
			if (DUQSys::sameString(module->name(), "NeutronSQ"))
			{
				// Retrieve the ReferenceData from the Module (as XYData)
				XYData& referenceData = GenericListHelper<XYData>::retrieve(duq.processingModuleData(), "ReferenceData", module->uniqueName(), XYData(), &found);
				if (!found)
				{
					Messenger::warn("Could not locate ReferenceData for target '%s'.\n", module->uniqueName());
					return false;
				}

				// Retrieve the PartialSet from the Module
				PartialSet& calcSQ = GenericListHelper<PartialSet>::retrieve(duq.processingModuleData(), "WeightedSQ", module->uniqueName(), PartialSet(), &found);
				if (!found)
				{
					Messenger::warn("Could not locate associated weighted neutron PartialSet for target '%s'.\n", module->uniqueName());
					return false;
				}

				error = referenceData.error(calcSQ.total());

				// Calculate difference
				XYData& differenceData = GenericListHelper<XYData>::realise(duq.processingModuleData(), CharString("DifferenceData_%s", module->uniqueName()), uniqueName());
				differenceData.setObjectName(CharString("%s//Difference//%s", uniqueName_.get(), module->uniqueName()));
				differenceData = referenceData;
				differenceData.addInterpolated(calcSQ.total(), -1.0);
			}
			else return Messenger::error("Unrecognised Module type '%s', so can't calculate error.", module->name());

			// Calculate and store the percentage error
			errors.addPoint(duq.iteration(), error);
		}


		/*
		 * Update our full scattering matrix, and use it to generate partials from the supplied reference data
		 */

		// Create temporary storage for our summed UnweightedSQ, and related quantities such as density and sum factors
		Array2D<XYData>& combinedUnweightedSQ = GenericListHelper< Array2D<XYData> >::realise(duq.processingModuleData(), "UnweightedSQ", uniqueName_, GenericItem::InRestartFileFlag);
		Array2D<double> combinedRho, combinedFactor, combinedCWeights;
		combinedUnweightedSQ.initialise(duq.nAtomTypes(), duq.nAtomTypes(), true);
		combinedRho.initialise(duq.nAtomTypes(), duq.nAtomTypes(), true);
		combinedFactor.initialise(duq.nAtomTypes(), duq.nAtomTypes(), true);
		combinedCWeights.initialise(duq.nAtomTypes(), duq.nAtomTypes(), true);
		combinedRho = 0.0;
		combinedFactor = 0.0;
		combinedCWeights = 0.0;

		// Set object names in combinedUnweightedSQ
		i = 0;
		for (AtomType* at1 = duq.atomTypes(); at1 != NULL; at1 = at1->next, ++i)
		{
			j = i;
			for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++j) combinedUnweightedSQ.ref(i,j).setObjectName(CharString("%s//UnweightedSQ//%s//%s-%s", uniqueName(), group->name(), at1->name(), at2->name()));
		}

		// Realise storage for generated S(Q), and reinitialise the scattering matrix
		Array2D<XYData>&  generatedSQ = GenericListHelper< Array2D<XYData> >::realise(duq.processingModuleData(), "GeneratedSQ", uniqueName_, GenericItem::InRestartFileFlag);
		scatteringMatrix_.initialise(duq.atomTypeList(), generatedSQ, uniqueName_, group->name());

		// Get factor with which to add data, based on the requested AugmentationStyle
		double dataFactor = 1.0;
		if (augmentationStyle == RefineModule::PartialsAugmentation) dataFactor = augmentationParam;

		// Add a row in the scattering matrix for each target in the group
		targetIterator.restart();
		while (Module* module = targetIterator.iterate())
		{
			// Retrieve the reference data and associated Weights matrix and source unweighted partials
			XYData& referenceData = GenericListHelper<XYData>::retrieve(duq.processingModuleData(), "ReferenceData", module->uniqueName(), XYData(), &found);
			if (!found) return Messenger::error("Couldn't locate ReferenceData for target '%s'.\n", module->uniqueName());
			Weights& weights = GenericListHelper<Weights>::retrieve(duq.processingModuleData(), "FullWeights", module->uniqueName(), Weights(), &found);
			if (!found) return Messenger::error("Couldn't locate Weights for target '%s'.\n", module->uniqueName());
			PartialSet& unweightedSQ = GenericListHelper<PartialSet>::retrieve(duq.processingModuleData(), "UnweightedSQ", module->uniqueName(), PartialSet(), &found);
			if (!found) return Messenger::error("Couldn't locate UnweightedSQ for target '%s'.\n", module->uniqueName());
			double rho = GenericListHelper<
			double>::retrieve(duq.processingModuleData(), "EffectiveRho", module->uniqueName(), 0.0, &found);
			if (!found) return Messenger::error("Couldn't locate EffectiveRho for target '%s'.\n", module->uniqueName());

			// Add a row to our scattering matrix
			if (!scatteringMatrix_.addReferenceData(referenceData, weights, dataFactor)) return Messenger::error("Failed to add target data '%s' to weights matrix.\n", module->uniqueName());

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

					combinedUnweightedSQ.ref(globalI, globalJ ).addInterpolated(unweightedSQ.partial(i,j), factor);
					combinedRho.ref(globalI, globalJ) += rho * factor;
					combinedFactor.ref(globalI, globalJ) += factor;
					combinedCWeights.ref(globalI, globalJ) += weights.concentrationWeight(i,j);
				}
			}
		}

		// Normalise the combined values
		for (i=0; i<duq.nAtomTypes(); ++i)
		{
			for (j=i; j<duq.nAtomTypes(); ++j)
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

		if (augmentationStyle == RefineModule::PartialsAugmentation)
		{
			// Create the array
			simulatedReferenceData_.createEmpty(combinedUnweightedSQ.linearArraySize());
			i = 0;
			for (AtomType* at1 = duq.atomTypes(); at1 != NULL; at1 = at1->next, ++i)
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
// 					data->setName(CharString("Simulated Partial %s-%s", at1->name(), at2->name()));

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
		scatteringMatrix_.generatePartials(generatedSQ);

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

		PartialSet& summedUnweightedGR = GenericListHelper<PartialSet>::realise(duq.processingModuleData(), "UnweightedGR", uniqueName(), GenericItem::InRestartFileFlag);
		if (!RDFModule::sumUnweightedGR(procPool, this, group, duq.processingModuleData(), summedUnweightedGR)) return false;

		/*
		 * Construct matrix of difference partials (deltaSQ) for all AtomTypes
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
				dSQ.setObjectName(CharString("%s//DeltaSQ//%s//%s-%s", uniqueName_.get(), group->name(), at1->name(), at2->name()));

				// Reset the difference partial
				dSQ.clear();

				// Create the difference partial, over the range permitted by both datasets
				const Array<double> x1 = generatedSQ.ref(i, j).constArrayX();
				const Array<double> y1 = generatedSQ.ref(i, j).constArrayY();
				XYData& simulatedSQ = combinedUnweightedSQ.ref(i,j);
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
			}
		}

		/*
		 * Create perturbations to interatomic potentials
		 */
		const double weighting = keywords_.asDouble("Weighting");
		Array2D<XYData>& deltaPhiR = GenericListHelper< Array2D<XYData> >::realise(duq.processingModuleData(), "DeltaPhiR", uniqueName_, GenericItem::InRestartFileFlag, &created);
		if (created) deltaPhiR.initialise(nTypes, nTypes, true);
		Array2D<XYData>& deltaGR = GenericListHelper< Array2D<XYData> >::realise(duq.processingModuleData(), "DeltaGR", uniqueName_, GenericItem::InRestartFileFlag, &created);
		if (created) deltaGR.initialise(nTypes, nTypes, true);
	// 	if (modifyBonds)
	// 	{
	// 		Array2D<XYData>& deltaGRBond = GenericListHelper< Array2D<XYData> >::realise(duq.processingModuleData(), "DeltaGRBond", uniqueName_, GenericItem::InRestartFileFlag, &created);
	// 		if (created) deltaGRBond.initialise(nTypes, nTypes, true);
	// 	}

		// Set up / updated minimum radii array, considering all involved Configurations
		Array2D<double>& minimumRadii = GenericListHelper< Array2D<double> >::realise(duq.processingModuleData(), "MinimumRadii", uniqueName_, GenericItem::InRestartFileFlag, &created);
		if (created) minimumRadii.initialise(nTypes, nTypes, true);
		if (autoMinimumRadii)
		{
			RefListIterator<Configuration,bool> configIterator(configs);
			while (Configuration* cfg = configIterator.iterate())
			{
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
						XYData& gr = summedUnweightedGR.unboundPartial(i, j);

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
				dPhiR.setObjectName(CharString("%s//DeltaPhiR//%s//%s-%s", uniqueName_.get(), group->name(), at1->name(), at2->name()));

				// Grab delta g(r) container and make sure its object name is set
				XYData& inversion = deltaGR.ref(i, j);
				inversion.setObjectName(CharString("%s//Inversion//%s//%s-%s", uniqueName_.get(), group->name(), at1->name(), at2->name()));

				// Set the default weighting factor for the pair potential addition
				weight = weighting;

	// 			// If we are modifying bond equilibrium distances, search for signatures in the deltaGR, do this now.
	// 			// Such signatures will be removed from the final dGR, and will thus not contribute to the PairPotential addition.
	// 			if (modifyBonds)
	// 			{
	// 				// Grab the deltaGRBond container and make sure its object name is set
	// 				Array2D<XYData>& deltaGRBonds = GenericListHelper< Array2D<XYData> >::retrieve(duq.processingModuleData(), "DeltaGRBond", uniqueName_);
	// 				XYData& deltaGRBond = deltaGRBonds.ref(i,j);
	// 				deltaGRBond.setObjectName(CharString("%s//DeltaGRBond//%s-%s", uniqueName_.get(), at1->name(), at2->name()));
	// 
	// 				if (!modifyBondTerms(duq, dGR, at1, at2, deltaGRBond)) return false;
	// 				dGR.addInterpolated(deltaGRBond, -1.0);
	// 			}

				// Create a perturbation
				if (inversionMethod == RefineModule::DirectFourierPotentialInversion)
				{
					// Copy the delta S(Q) and do the inverse FT to get the delta [g(r) - 1]
					inversion = deltaSQ.ref(i, j);
					inversion.sineFT(1.0 / (2 * PI * PI * combinedRho.ref(i,j)), ppDelta, ppDelta, ppRange, windowFunction);

					dPhiR = inversion;
					dPhiR.arrayY() *= -1.0;
				}
				else if (inversionMethod == RefineModule::DirectGaussianPotentialInversion)
				{
					// Perform a Gaussian fit and do the inverse FT to get the delta [g(r) - 1]
					GaussFit gaussFit(deltaSQ.ref(i, j));
					double error = gaussFit.construct(0.5);
					Messenger::print("Fitted function has error of %f%% with original delta S(Q) (nGaussians = %i).\n", error, gaussFit.nGaussians());

					// Store fitted parameters
					// If the fit parameter arrays already exist in the module data, retrieve and set them now.
					GenericListHelper< Array<double> >::realise(duq.processingModuleData(), CharString("%s-%s-GaussianX", at1->name(), at2->name()), uniqueName_, GenericItem::InRestartFileFlag) = gaussFit.x();
					GenericListHelper< Array<double> >::realise(duq.processingModuleData(), CharString("%s-%s-GaussianA", at1->name(), at2->name()), uniqueName_, GenericItem::InRestartFileFlag) = gaussFit.A();
					GenericListHelper< Array<double> >::realise(duq.processingModuleData(), CharString("%s-%s-GaussianC", at1->name(), at2->name()), uniqueName_, GenericItem::InRestartFileFlag) = gaussFit.c();

					// Fourier transform the approximation, and store this as our inversion
					inversion = gaussFit.fourierTransform(ppDelta, ppDelta, ppRange);
					dPhiR = inversion;
					dPhiR.arrayY() *= -1.0;
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
					cr = calculateCR(deltaSQ.ref(i, j), 1.0 / (2.0*PI*PI*combinedRho.ref(i,j)), ppDelta, ppDelta, ppRange, windowFunction);

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

				// Smooth phi(r)?
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

