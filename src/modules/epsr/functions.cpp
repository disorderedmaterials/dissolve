/*
	*** EPSR Module - Functions
	*** src/modules/epsr/functions.cpp
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

#include "main/dissolve.h"
#include "modules/epsr/epsr.h"
#include "classes/atomtype.h"
#include "math/gaussfit.h"
#include "math/poissonfit.h"
#include "genericitems/listhelper.h"

// Return list of target Modules / data for refeinement
const RefDataList<Module,ModuleGroup*>& EPSRModule::allTargets() const
{
	return groupedTargets_.modules();
}

// Return grouped target Modules
const ModuleGroups& EPSRModule::groupedTargets() const
{
	return groupedTargets_;
}

// Add target Modules
void EPSRModule::addTargets(RefList<Module> targets, const char* groupName)
{
	for (Module* module : targets) groupedTargets_.addModule(module, groupName);

	// Must flag that the associated keyword has been set by other means
	if (targets.nItems() > 0) keywords_.hasBeenSet("Target");
}

// Create / retrieve arrays for storage of empirical potential coefficients
Array2D< Array<double> >& EPSRModule::potentialCoefficients(Dissolve& dissolve, const int nAtomTypes, const int ncoeffp)
{
	Array2D< Array<double> >& coefficients = GenericListHelper< Array2D< Array<double> > >::realise(dissolve.processingModuleData(), "PotentialCoefficients", uniqueName_, GenericItem::InRestartFileFlag);
	int arrayNCoeffP = (coefficients.nRows() && coefficients.nColumns() ? coefficients.at(0,0).nItems() : 0);
	if ((coefficients.nRows() != nAtomTypes) || (coefficients.nColumns() != nAtomTypes) || ((ncoeffp != -1) && (ncoeffp != arrayNCoeffP)))
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
bool EPSRModule::generateEmpiricalPotentials(Dissolve& dissolve, EPSRModule::ExpansionFunctionType functionType, double averagedRho, int ncoeffp, double rminpt, double rmaxpt, double sigma1, double sigma2)
{
	const int nAtomTypes = dissolve.nAtomTypes();
	int i, j;

	// Get coefficients array
	Array2D< Array<double> >& coefficients = potentialCoefficients(dissolve, nAtomTypes, ncoeffp);

	i = 0;
	for (AtomType* at1 = dissolve.atomTypes().first(); at1 != NULL; at1 = at1->next(), ++i)
	{
		j = i;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next(), ++j)
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
				// We pass 1.0/rho as the factor to PossonFit::approximation() - this is the factor of rho not present in our denominator
				PoissonFit generator(ep);
				generator.set(FunctionSpace::ReciprocalSpace, rmaxpt, potCoeff, sigma1, sigma2);
				ep = generator.approximation(FunctionSpace::RealSpace, 1.0/averagedRho, 0.0, dissolve.pairPotentialDelta(), dissolve.pairPotentialRange());
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

// Generate and return single empirical potential function
Data1D EPSRModule::generateEmpiricalPotentialFunction(Dissolve& dissolve, int i, int j, int n)
{
	const int nAtomTypes = dissolve.nAtomTypes();
	ExpansionFunctionType functionType = keywords_.enumeration<EPSRModule::ExpansionFunctionType>("ExpansionFunction");
	const double gsigma1 = keywords_.asDouble("GSigma1");
	const double gsigma2 = keywords_.asDouble("GSigma2");
	int ncoeffp = keywords_.asInt("NCoeffP");
	const double psigma1 = keywords_.asDouble("PSigma1");
	const double psigma2 = keywords_.asDouble("PSigma2");
	const double qMax = keywords_.asDouble("QMax");
	const double qMin = keywords_.asDouble("QMin");
	double rmaxpt = keywords_.asDouble("RMaxPT");
	double rminpt = keywords_.asDouble("RMinPT");

	// EPSR constants
	const int mcoeff = 200;

	// Calculate some values if they were not provided
	if (rmaxpt < 0.0) rmaxpt = dissolve.pairPotentialRange();
	if (rminpt < 0.0) rminpt = rmaxpt - 2.0;
	if (ncoeffp <= 0) ncoeffp = std::min(int(10.0*rmaxpt+0.0001), mcoeff);

	// Get coefficients array
	Array2D< Array<double> >& coefficients = potentialCoefficients(dissolve, nAtomTypes);
	Array<double>& potCoeff = coefficients.at(i, j);

	// Regenerate empirical potential from the stored coefficients
	Data1D result;
	if (functionType == EPSRModule::GaussianExpansionFunction)
	{
		// Construct our fitting object and generate the potential using it
		GaussFit generator(result);
		generator.set(rmaxpt, potCoeff, gsigma1);
		result = generator.singleFunction(n, FunctionSpace::RealSpace, 1.0, 0.0, dissolve.pairPotentialDelta(), dissolve.pairPotentialRange(), gsigma2 / gsigma1);
	}
	else if (functionType == EPSRModule::PoissonExpansionFunction)
	{
		// Construct our fitting object and generate the potential using it
		PoissonFit generator(result);
		generator.set(FunctionSpace::ReciprocalSpace, rmaxpt, potCoeff, psigma1, psigma2);
		result = generator.singleFunction(n, FunctionSpace::RealSpace, 1.0, 0.0, dissolve.pairPotentialDelta(), dissolve.pairPotentialRange());
	}

	// Multiply by truncation function
	truncate(result, rminpt, rmaxpt);

	return result;
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
	for (AtomType* at1 = dissolve.atomTypes().first(); at1 != NULL; at1 = at1->next(), ++i)
	{
		int j = i;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next(), ++j)
		{
			Array<double>& potCoeff = coefficients.at(i, j);

			double cMin = potCoeff.nItems() == 0 ? 0.0 : potCoeff.constAt(0);
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
