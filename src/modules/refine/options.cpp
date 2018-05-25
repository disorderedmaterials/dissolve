/*
	*** Refine Module - Options
	*** src/modules/refine/options.cpp
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

#include "modules/refine/refine.h"
#include "module/list.h"
#include "module/keywordtypes.h"
#include "base/lineparser.h"

// Potential Inversion Method
const char* PotentialInversionMethodKeywords[] = { "Direct", "Gaussian", "PY", "HNC" };

// Convert text string to PotentialGenerationType
RefineModule::PotentialInversionMethod RefineModule::potentialInversionMethod(const char* s)
{
	for (int n=0; n<RefineModule::nPotentialInversionMethods; ++n) if (DissolveSys::sameString(s, PotentialInversionMethodKeywords[n])) return (RefineModule::PotentialInversionMethod) n;
	return RefineModule::nPotentialInversionMethods;
}

// Convert PotentialGenerationType to text string
const char* RefineModule::potentialInversionMethod(RefineModule::PotentialInversionMethod pim)
{
	return PotentialInversionMethodKeywords[pim];
}

// Matrix Augmentation Style
const char* MatrixAugmentationStyleKeywords[] = { "None", "Partials" };

// Convert text string to MatrixAugmentationStyle
RefineModule::MatrixAugmentationStyle RefineModule::matrixAugmentationStyle(const char* s)
{
	for (int n=0; n<RefineModule::nMatrixAugmentationStyles; ++n) if (DissolveSys::sameString(s, MatrixAugmentationStyleKeywords[n])) return (RefineModule::MatrixAugmentationStyle) n;
	return RefineModule::nMatrixAugmentationStyles;
}

// Convert MatrixAugmentationStyle to text string
const char* RefineModule::matrixAugmentationStyle(RefineModule::MatrixAugmentationStyle mas)
{
	return MatrixAugmentationStyleKeywords[mas];
}

// Set up keywords for Module
void RefineModule::setUpKeywords()
{
	keywords_.add(new CharStringModuleKeyword("Partials", RefineModule::nMatrixAugmentationStyles, MatrixAugmentationStyleKeywords), "Augmentation", "Style used to augment (overdetermine) scattering matrix");
	keywords_.add(new DoubleModuleKeyword(0.9), "AugmentationParam", "Parameter used to in augmentation (overdetermination) of scattering matrix (dependent on augmentation style selected)");
	keywords_.add(new BoolModuleKeyword(true), "AutoMinimumRadius", "Automatically determine minimum radii between atom types for potential generation");
	keywords_.add(new BoolModuleKeyword(true), "DeltaPhiRSmoothing", "Whether to smooth generated phi(r)");
	keywords_.add(new IntegerModuleKeyword(5, 1, 10), "DeltaPhiRSmoothK", "Iterations of KZ smoothing to apply to generated potentials");
	keywords_.add(new IntegerModuleKeyword(5, 3, 9), "DeltaPhiRSmoothM", "Smoothing length for moving average calculation in KZ filter applied to generated potentials");
	keywords_.add(new DoubleModuleKeyword(0.005), "ErrorStabilityThreshold", "Threshold value at which error is deemed stable over the defined windowing period", "<value[0.0-1.0]>");
	keywords_.add(new IntegerModuleKeyword(10), "ErrorStabilityWindow", "Number of points over which to assess the stability of errors");
	keywords_.add(new DoubleModuleKeyword(0.5, 0.01, 100.0), "GaussianAccuracy", "Requested percentage error of Gaussian approximation (if InversionMethod == Gaussian)");
	keywords_.add(new CharStringModuleKeyword("Gaussian", RefineModule::nPotentialInversionMethods, PotentialInversionMethodKeywords), "InversionMethod", "Potential inversion method to employ");
	keywords_.add(new DoubleModuleKeyword(0.9, 0.0, 5.0), "MinimumRadius", "Minimum value of r at which additional potential is allowed to take effect (neglecting width of truncation strip)");
	keywords_.add(new DoubleModuleKeyword(3.0, 0.0, 100.0), "MaximumRadius", "Maximum value of r (if AutoMinimumRadii = true) at which additional potential is zeroed");
// 	keywords_.add(new BoolModuleKeyword(false), "ModifyBonds", "Modify equilibrium distances of bonds based on signatures in difference functions");
	keywords_.add(new BoolModuleKeyword(true), "ModifyPotential", "Whether to apply generated perturbations to interatomic potentials");
	keywords_.add(new BoolModuleKeyword(true), "OnlyWhenEnergyStable", "Assesses the energy of all involved Configurations, refining the potential only when all their total energies are stable");
	keywords_.add(new BoolModuleKeyword(true), "OnlyWhenErrorStable", "Assesses the error of the fits to the reference data, refining the potential only when all errors are stable");
	keywords_.add(new DoubleModuleKeyword(3.0, -1.0), "PhiMax", "Limit of magnitude of additional potential for any one pair potential");
	keywords_.add(new DoubleModuleKeyword(30.0, -1.0), "QMax", "Maximum Q value over which to generate potentials from total scattering data");
	keywords_.add(new DoubleModuleKeyword(0.1, -1.0), "QMin", "Minimum Q value over which to generate potentials from total scattering data");
	keywords_.add(new ComplexModuleKeyword(1,2), "Target", "Add specified Module (and it's Reference data) as a fitting target", "<ModuleName> [GroupName]");
	keywords_.add(new DoubleModuleKeyword(0.2, 0.01, 1.0), "TruncationWidth", "Width of truncation zone, below the minimum radius, over which additional potential smoothly decreases to zero");
	keywords_.add(new DoubleModuleKeyword(1.0, 0.0, 10.0), "Weighting", "Fractional (maximal) amounts of generated perturbations to apply to pair potentials");
	keywords_.add(new WindowFunctionModuleKeyword(WindowFunction(WindowFunction::SineWindow)), "WindowFunction", "Window function to apply when back-transforming delta S(Q) to g(r)");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int RefineModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix)
{
	if (DissolveSys::sameString(parser.argc(0), "Target")) return addTarget(parser.argc(1), parser.hasArg(2) ? parser.argc(2) : "Default");
	else return -1;

	return true;
}
