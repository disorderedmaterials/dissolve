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

// Return list of target Modules / data for refeinement
const RefList<Module,bool>& EPSRModule::allTargets() const
{
	return groupedTargets_.modules();
}

// Return grouped target Modules
const ModuleGroups& EPSRModule::groupedTargets() const
{
	return groupedTargets_;
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
bool EPSRModule::generateEmpiricalPotentials(Dissolve& dissolve, EPSRModule::ExpansionFunctionType functionType, int ncoeffp, double rminpt, double rmaxpt, double sigma1, double sigma2)
{
	const int nAtomTypes = dissolve.nAtomTypes();
	int i, j;

	// Get coefficients array
	Array2D< Array<double> >& coefficients = potentialCoefficients(dissolve, nAtomTypes, ncoeffp);

	i = 0;
	for (AtomType* at1 = dissolve.atomTypes().first(); at1 != NULL; at1 = at1->next, ++i)
	{
		j = i;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++j)
		{
			Array<double>& potCoeff = coefficients.at(i, j);

			// Regenerate empirical potential from the stored coefficients
			Data1D ep;
			if (functionType == EPSRModule::GaussianExpansionFunction)
			{
				// Construct our fitting object and generate the potential using it
				GaussFit generator(ep);
				generator.set(rmaxpt, potCoeff, sigma1);
				ep = generator.approximation(FunctionSpace::RealSpace, 1.0, 0.0, dissolve.pairPotentialDelta(), dissolve.pairPotentialRange(), sigma2 / sigma1);
			}
			else if (functionType == EPSRModule::PoissonExpansionFunction)
			{
				// Construct our fitting object and generate the potential using it
				PoissonFit generator(ep);
				generator.set(FunctionSpace::ReciprocalSpace, rmaxpt, potCoeff, sigma1, sigma2);
				ep = generator.approximation(FunctionSpace::RealSpace, 1.0, 0.0, dissolve.pairPotentialDelta(), dissolve.pairPotentialRange());
			}

			// Multiply by truncation function
			truncate(ep, rminpt, rmaxpt);

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

	return true;
}

// Calculate absolute energy of empirical potentials
double EPSRModule::absEnergyEP(Dissolve& dissolve)
{
	/*
	 * Routine from EPSR25.
	 * Loop over sets of empirical potential coefficients and find the maximal/minimal values.
	 * Return the largest range we find.
	 */

	// Get coefficients array
	Array2D< Array<double> >& coefficients = potentialCoefficients(dissolve, dissolve.nAtomTypes());

	double absEnergyEP = 0.0;

	int i = 0;
	for (AtomType* at1 = dissolve.atomTypes().first(); at1 != NULL; at1 = at1->next, ++i)
	{
		int j = i;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++j)
		{
			Array<double>& potCoeff = coefficients.at(i, j);

			double cMin = potCoeff.constAt(0);
			double cMax = cMin;
			for (int n=1; n<potCoeff.nItems(); ++n)
			{
				if (potCoeff.constAt(n) < cMin) cMin = potCoeff.constAt(n);
				if (potCoeff.constAt(n) > cMax) cMax = potCoeff.constAt(n); 
			}

			double range = cMax - cMin;
			if (range > absEnergyEP) absEnergyEP = range;

			// Output information
			Messenger::print("  abs_energy_ep>    %4s %4s %12.6f\n", at1->name(), at2->name(), range);
		}
	}

	return absEnergyEP;
}

// Truncate the supplied data
void EPSRModule::truncate(Data1D& data, double rMin, double rMax)
{
	// Replicates the EPSR25 truncate(xx,rminpt,rmaxpt) function applied over a whole dataset.
	double x;
	Array<double>& y = data.values();
	const double decay = rMax - rMin;
	for (int n=0; n<data.nValues(); ++n)
	{
		x = data.xAxis(n);

		if (x >= rMax) y[n] = 0.0;
// 		else if (x <= rMin) y[n] = y[n];
		else if (x > rMin) y[n] *= 0.5 * (1.0+cos( ((x-rMin)*PI)/decay ));
	}
}
