/*
	*** EPSR Module - Functions
	*** src/modules/epsr/functions.cpp
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

#include "main/dissolve.h"
#include "modules/epsr/epsr.h"
#include "classes/atomtype.h"
#include "math/gaussfit.h"
#include "math/poissonfit.h"
#include "templates/genericlisthelper.h"

// Add Module target to specified group
bool EPSRModule::addTarget(const char* moduleTarget, const char* group)
{
	// First, find the named Module
	Module* module = ModuleList::findInstanceByUniqueName(moduleTarget);
	if (!module) return Messenger::error("Couldn't find Module '%s' to add to EPSRModule's list of targets.\n", moduleTarget);

	// Check on the type of the Module given... if OK, add to the specified group
	if (DissolveSys::sameString(module->name(), "NeutronSQ")) Messenger::print("Adding NeutronSQ target '%s' to '%s'.\n", moduleTarget, uniqueName());
	else return Messenger::error("Can't use Module of type '%s' as a fitting target.\n", module->name());

	// Does the specified group exist?
	ModuleGroup* moduleGroup;
	for (moduleGroup = targetGroups_.first(); moduleGroup != NULL; moduleGroup = moduleGroup->next) if (DissolveSys::sameString(moduleGroup->name(), group)) break;
	if (moduleGroup == NULL)
	{
		moduleGroup = new ModuleGroup(group);
		targetGroups_.own(moduleGroup);
	}

	targets_.add(module);
	moduleGroup->add(module);

	return true;
}

// Return list of target Modules / data for fitting process
const RefList<Module,bool>& EPSRModule::targets() const
{
	return targets_;
}

// Return list of target groups defined
const List<ModuleGroup>& EPSRModule::targetGroups() const
{
	return targetGroups_;
}

// Create / retrieve arrays for storage of empirical potential coefficients
Array2D< Array<double> >& EPSRModule::potentialCoefficients(Dissolve& dissolve, const int nAtomTypes, const int ncoeffp)
{
	Array2D< Array<double> >& coefficients = GenericListHelper< Array2D< Array<double> > >::realise(dissolve.processingModuleData(), "PotentialCoefficients", uniqueName_, GenericItem::InRestartFileFlag);
	if ((coefficients.nRows() != nAtomTypes) || (coefficients.nColumns() != nAtomTypes))
	{
		coefficients.initialise(nAtomTypes, nAtomTypes, true);
		for (int n=0; n<coefficients.linearArraySize(); ++n)
		{
			coefficients.linearArray()[n].initialise(ncoeffp);
			coefficients.linearArray()[n] = 0.0;
		}
	}

	return coefficients;
}

// Generate empirical potentials from current coefficients
bool EPSRModule::generateEmpiricalPotentials(Dissolve& dissolve, EPSRModule::ExpansionFunctionType functionType, int ncoeffp, double rmaxpt, double sigma1, double sigma2)
{
	const int nAtomTypes = dissolve.nAtomTypes();
	int i, j;

	// Get coefficients array
	Array2D< Array<double> >& coefficients = potentialCoefficients(dissolve, nAtomTypes, ncoeffp);

	i = 0;
	for (AtomType* at1 = dissolve.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++i)
	{
		j = i;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++j)
		{
			Array<double>& potCoeff = coefficients.ref(i, j);

			// Regenerate empirical potential from the stored coefficients
			XYData ep;
			if (functionType == EPSRModule::GaussianExpansionFunction)
			{
				// Construct our fitting object and generate the potential using it
				GaussFit generator(ep);
				generator.set(rmaxpt, potCoeff, sigma1);
				ep = generator.approximation(FunctionSpace::RealSpace, 1.0, 0.0, dissolve.pairPotentialDelta(), dissolve.pairPotentialRange(), sigma2 / sigma1);
				ep.save(CharString("EP.%i%i", i, j));
			}
			else if (functionType == EPSRModule::PoissonExpansionFunction)
			{
				// Construct our fitting object and generate the potential using it
				PoissonFit generator(ep);
				generator.set(FunctionSpace::ReciprocalSpace, rmaxpt, potCoeff, sigma1, sigma2);
				ep = generator.approximation(FunctionSpace::RealSpace, 1.0, 0.0, dissolve.pairPotentialDelta(), dissolve.pairPotentialRange());
			}
		
			// Grab pointer to the relevant pair potential
			PairPotential* pp = dissolve.pairPotential(at1, at2);
			if (!pp)
			{
				Messenger::error("Failed to find PairPotential for AtomTypes '%s' and '%s'.\n", at1->name(), at2->name());
				return false;
			}

			pp->setUAdditional(ep);
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

	return true;
}
