/*
	*** Refine Module - Initialisation
	*** src/modules/refine/init.cpp
	Copyright T. Youngs 2012-2019

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
#include "keywords/types.h"

// Return enum options for PotentialInversionMethod
EnumOptions<RefineModule::PotentialInversionMethod> RefineModule::potentialInversionMethods()
{
	static EnumOptionsList PotentialInversionMethodOptions = EnumOptionsList() <<
		EnumOption(RefineModule::DirectFourierPotentialInversion,	"Direct") <<
		EnumOption(RefineModule::DirectGaussianPotentialInversion,	"Gaussian") <<
		EnumOption(RefineModule::PercusYevickPotentialInversion,	"PY") <<
		EnumOption(RefineModule::HypernettedChainPotentialInversion,	"HNC");

	static EnumOptions<RefineModule::PotentialInversionMethod> options("PotentialInversionMethod", PotentialInversionMethodOptions, RefineModule::DirectGaussianPotentialInversion);

	return options;
}


// Return enum options for MatrixAugmentationStyle
EnumOptions<RefineModule::MatrixAugmentationStyle> RefineModule::matrixAugmentationStyles()
{
	static EnumOptionsList MatrixAugmentationStyleOptions = EnumOptionsList() <<
		EnumOption(RefineModule::NoAugmentation,	"None") <<
		EnumOption(RefineModule::PartialsAugmentation,	"Partials");

	static EnumOptions<RefineModule::MatrixAugmentationStyle> options("MatrixAugmentationStyle", MatrixAugmentationStyleOptions, RefineModule::PartialsAugmentation);

	return options;
}

// Perform any necessary initialisation for the Module
void RefineModule::initialise()
{
	groupedTargets_.addAllowedModuleType("NeutronSQ");

	keywords_.add(new EnumOptionsKeyword<RefineModule::MatrixAugmentationStyle>(matrixAugmentationStyles() = RefineModule::PartialsAugmentation), "Augmentation", "Style used to augment (overdetermine) scattering matrix");
	keywords_.add(new DoubleKeyword(0.9), "AugmentationParam", "Parameter used to in augmentation (overdetermination) of scattering matrix (dependent on augmentation style selected)");
	keywords_.add(new BoolKeyword(true), "AutoMinimumRadius", "Automatically determine minimum radii between atom types for potential generation");
	keywords_.add(new BoolKeyword(true), "DeltaPhiRSmoothing", "Whether to smooth generated phi(r)");
	keywords_.add(new IntegerKeyword(5, 1, 10), "DeltaPhiRSmoothK", "Iterations of KZ smoothing to apply to generated potentials");
	keywords_.add(new IntegerKeyword(5, 3, 9), "DeltaPhiRSmoothM", "Smoothing length for moving average calculation in KZ filter applied to generated potentials");
	keywords_.add(new DoubleKeyword(0.005), "ErrorStabilityThreshold", "Threshold value at which error is deemed stable over the defined windowing period", "<value[0.0-1.0]>");
	keywords_.add(new IntegerKeyword(10), "ErrorStabilityWindow", "Number of points over which to assess the stability of errors");
	keywords_.add(new DoubleKeyword(0.5, 0.01, 100.0), "GaussianAccuracy", "Requested percentage error of Gaussian approximation (if InversionMethod == Gaussian)");
	keywords_.add(new EnumOptionsKeyword<RefineModule::PotentialInversionMethod>(potentialInversionMethods() = RefineModule::DirectGaussianPotentialInversion), "InversionMethod", "Potential inversion method to employ");
	keywords_.add(new DoubleKeyword(0.9, 0.0, 5.0), "MinimumRadius", "Minimum value of r at which additional potential is allowed to take effect (neglecting width of truncation strip)");
	keywords_.add(new DoubleKeyword(3.0, 0.0, 100.0), "MaximumRadius", "Maximum value of r (if AutoMinimumRadii = true) at which additional potential is zeroed");
// 	keywords_.add(new BoolKeyword(false), "ModifyBonds", "Modify equilibrium distances of bonds based on signatures in difference functions");
	keywords_.add(new BoolKeyword(true), "ModifyPotential", "Whether to apply generated perturbations to interatomic potentials");
	keywords_.add(new BoolKeyword(true), "OnlyWhenEnergyStable", "Assesses the energy of all involved Configurations, refining the potential only when all their total energies are stable");
	keywords_.add(new BoolKeyword(false), "OnlyWhenErrorStable", "Assesses the error of the fits to the reference data, refining the potential only when all errors are stable");
	keywords_.add(new DoubleKeyword(3.0, -1.0), "PhiMax", "Limit of magnitude of additional potential for any one pair potential");
	keywords_.add(new DoubleKeyword(30.0, -1.0), "QMax", "Maximum Q value over which to generate potentials from total scattering data");
	keywords_.add(new DoubleKeyword(0.1, -1.0), "QMin", "Minimum Q value over which to generate potentials from total scattering data");
	keywords_.add(new ModuleGroupsKeyword(groupedTargets_), "Target", "Add specified Module (and it's Reference data) as a refinement target", "<ModuleName> [GroupName]");
	keywords_.add(new DoubleKeyword(0.2, 0.01, 1.0), "TruncationWidth", "Width of truncation zone, below the minimum radius, over which additional potential smoothly decreases to zero");
	keywords_.add(new DoubleKeyword(1.0, 0.0, 10.0), "Weighting", "Fractional (maximal) amounts of generated perturbations to apply to pair potentials");
	keywords_.add(new WindowFunctionKeyword(WindowFunction(WindowFunction::SineWindow)), "WindowFunction", "Window function to apply when back-transforming delta S(Q) to g(r)");
}

