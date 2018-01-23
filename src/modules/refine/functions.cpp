/*
	*** Refine Module - Functions
	*** src/modules/refine/functions.cpp
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

#include "main/duq.h"
#include "modules/refine/refine.h"
#include "classes/atomtype.h"
#include "classes/masterintra.h"
#include "classes/speciesbond.h"
#include "templates/praxis.h"

// Calculate c(r) from supplied S(Q)
XYData RefineModule::calculateCR(const XYData& sq, double normFactor, double rMin, double rStep, double rMax, WindowFunction windowFunction, BroadeningFunction broadening, bool unbroaden)
{
	// Create working array
	XYData cr;

	// Assume deltQ is the difference between the first two points
	double deltaQ = sq.x(1) - sq.x(0);

	// Set up window function
	windowFunction.setUp(sq);

	// Get delta0
	const double delta0 = PI / sq.xLast();

	int nQ = sq.nPoints();

	// Perform Fourier sine transform, apply general and omega-dependent broadening, as well as window function
	double ft, window, broaden;
	double omega = rMin;
	while (omega <= rMax)
	{
		ft = 0.0;
		for (int m=0; m<nQ; ++m)
		{
			// Get window value at this point in the function
			window = windowFunction.y(sq.x(m), omega);

			// Calculate broadening
			broaden = (unbroaden ? 1.0 / broadening.yFT(sq.x(m), omega) : broadening.yFT(sq.x(m), omega));

			ft += broaden * window * sq.y(m) * deltaQ * ((sin(sq.x(m)*omega) * sq.x(m)) / (sq.y(m)+1.0));
		}

		// Normalise
		if (omega > 0.0) ft /= omega;
		cr.addPoint(omega, ft);
		
		omega += rStep;
	}

	// Apply normalisation factor
	cr.arrayY() *= normFactor;

	return cr;
}

// Determine modification to bonds based on supplied delta g(r), returning features extracted from deltaGR
bool RefineModule::modifyBondTerms(DUQ& duq, const XYData& deltaGR, AtomType* typeI, AtomType* typeJ, XYData& deltaBond)
{
	// exp(-(((x-r)-delta)**2)/width**2)-exp(-(((x-r)+delta)**2)/width**2) w l

	// Scan through master bond terms searching for those that involve the AtomTypes specified
	const int idI = typeI->index();
	const int idJ = typeJ->index();
	RefList<MasterIntra,double> masterBonds;
	for (MasterIntra* b = duq.masterBonds(); b != NULL; b = b->next) if (b->usageCount(idI, idJ) > 0) masterBonds.add(b);

	/*
	 * We now have a reference list of MasterIntra bond terms that involve these two AtomTypes.
	 * For each we will try and fit our 'delta' function to the deltaGR, in and around the equilibrium distance
	 * of the MasterIntra term.
	 * XXX
	 * We will impose some restrictions on the fitting, so as to limit the fitting area close to the current value of
	 * the masterBond's equilibrium value.  Our fitFunction() will impose a quadractic penalty function about the 
	 */
	
	// Create a minimiser, and add our target variables - the order here is important, as the costFunction() is just
	// provided with an array of doubles in the same order.
	double xCentre, delta, width, AL, AC, AR;
	Praxis<RefineModule> minimiserLCR(*this, &RefineModule::costFunction3Exp);
	minimiserLCR.addTarget(xCentre);
	minimiserLCR.addTarget(delta);
	minimiserLCR.addTarget(width);
	minimiserLCR.addTarget(AL);
	minimiserLCR.addTarget(AC);
	minimiserLCR.addTarget(AR);

	Praxis<RefineModule> minimiserLR(*this, &RefineModule::costFunction2Exp);
	minimiserLR.addTarget(xCentre);
	minimiserLR.addTarget(delta);
	minimiserLR.addTarget(width);
	minimiserLR.addTarget(AL);
	minimiserLR.addTarget(AC);
	minimiserLR.addTarget(AR);

	fitData_ = deltaGR;
	xCentreDeltaLimit_ = 0.1;

	// Set up the deltaBond data
	deltaBond = deltaGR;
	deltaBond.arrayY() = 0.0;

	// Loop over reference list of MasterIntra
	RefListIterator<MasterIntra,double> bondIterator(masterBonds);
	while (MasterIntra* masterIntra = bondIterator.iterate())
	{
		// Set our variables ready for the fit
		if (masterIntra->form() == SpeciesBond::HarmonicForm) xCentre = masterIntra->parameter(1);
		else
		{
			Messenger::error("Functional form of MasterIntra '%s' not recognised, so can't extract equilibrium value.\n", masterIntra->name());
			return false;
		}
		xCentreStart_ = xCentre;
		delta = 0.1;
		width = 0.01;
		AL = -1.0;
		AC = 0.0;
		AR = 1.0;

		Messenger::print("Examining master bond term '%s' - current equilibrium value is %f Angstroms...\n", masterIntra->name(), xCentre);
		double fitValue = minimiserLCR.minimise();
		Messenger::print("Fit (cost = %f): x = %f, delta = %f, width = %f, AL = %f, AC = %f, AR = %f\n", fitValue, xCentre, delta, width, AL, AC, AR);

		// Check the final fit value
		if (fitValue <= 5.0)
		{
			// Fit is OK - try to work out what it means!
			if (DUQMath::sgn(AL) != DUQMath::sgn(AR))
			{
				// AL and AR have opposite signs which probably means a sine-like curve indicating a mismatch of peak positions
				// Check the magnitude of AC - if it is sufficiently small we will assume this is the case
				if (fabs(AC) < 0.05*fabs(AL))
				{
					// Adjust the equilibrium bond length...
					double newEq = xCentreStart_ + 0.1 * (xCentre-xCentreStart_);
					Messenger::print("Fitting suggests mismatch of equilibrium bond lengths - adjusting from %f to %f Angstroms.\n", xCentreStart_, newEq);				
					if (masterIntra->form() == SpeciesBond::HarmonicForm) masterIntra->setParameter(1, newEq);
				}
				else continue;

				Messenger::print("Resulting magnitude of fit is below threshold (A = %f), so no modification will be performed.\n", AL);
				continue;
			}
			else if (DUQMath::sgn(AC) != DUQMath::sgn(AL))
			{
				// AL and AR have the same sign, and AC is different, which may indicate the force constant is wrong
				if (fabs(AC) > fabs(AL))
				{
					// Adjust the bond force constant...
					double newK;
					if (masterIntra->form() == SpeciesBond::HarmonicForm) newK = masterIntra->parameter(0)* 0.9;
					Messenger::print("Fitting suggests wrong force constant - adjusting to %f kJ/mol/A**2.\n", newK);				
					if (masterIntra->form() == SpeciesBond::HarmonicForm) masterIntra->setParameter(0, newK);
				}
				else continue;
			}
		}
		else
		{
			// Try again, but with a two-exponential fit

			// Set our variables ready for the fit
			if (masterIntra->form() == SpeciesBond::HarmonicForm) xCentre = masterIntra->parameter(1);
			else
			{
				Messenger::error("Functional form of MasterIntra '%s' not recognised, so can't extract equilibrium value.\n", masterIntra->name());
				return false;
			}
			delta = 0.1;
			width = 0.01;
			AL = -1.0;
			AC = 0.0;
			AR = 1.0;
			fitValue = minimiserLR.minimise();

			if (fitValue > 5.0)
			{
				Messenger::print("Final cost value is above threshold (cost = %f) so no modification will be performed.\n", fitValue);
				continue;
			}

			// Adjust the equilibrium bond length...
			double newEq = xCentreStart_ + 0.1 * (xCentre-xCentreStart_);
			Messenger::print("Exp2 fitting suggests mismatch of equilibrium bond lengths - adjusting from %f to %f Angstroms.\n", xCentreStart_, newEq);				
			if (masterIntra->form() == SpeciesBond::HarmonicForm) masterIntra->setParameter(1, newEq);
		}

		// One of our masterbonds has provided a good fit to the data - we may as well stop here
		// TODO Need to test how this will affect systems where multiple bonds contribute to the same peak.
		sumFitEquation(deltaBond, xCentre, delta, width, AL, AC, AR);

		break;
	}

	return true;
}

// Return value of fit equation at specified x, given specified parameters
double RefineModule::fitEquation(double x, double xCentre, double delta, double width, double AL, double AC, double AR)
{
	/*
	 * Fit equation the of sum of two offset Gaussians around a central Gaussian:
	 * 
	 * 		   -((x - xI) - delta)**2	     -(x - xI)**2	     -((x - xI) + delta)**2
	 * f(x) = AL * exp ---------------------- + AC * exp ------------ + AR * exp ----------------------
	 * 	     		  width**2		       width**2			    width**2
	 * 
	 */

	const double dx = x - xCentre;
	const double widthSquared = width * width;
	return AL*exp(-((dx-delta)*(dx-delta))/widthSquared) + AC*exp(-(dx*dx)/widthSquared) + AR*exp(-((dx+delta)*(dx+delta))/widthSquared);
}

// Three-exponential, 6-parameter cost function for modifyBondTerms() fitting
double RefineModule::costFunction3Exp(double params[], int n)
{
	/*
	 * params[] are as follows:
	 *
	 * params[0] = x intercept of function
	 * params[1] = position difference about x intercept, delta
	 * params[2] = width-squared of function lobes
	 * params[3] = amplitude of left function lobe
	 * params[4] = amplitude of central function lobe
	 * params[5] = amplitude of right function lobe
	 * 
	 */

	// We will evaluate the fit over r +/- window
	const double window = 0.5;
	const double windowDelta = 0.01;
	double sos = 0.0, delta, func;
	int nPoints = 0;
	double x = params[0] - window;
	const double xMax = params[0] + window;
	while (x <= xMax)
	{
		// Check x against limits of function
		if (x < fitData_.xFirst())
		{
			x += windowDelta;
			continue;
		}
		else if (x > fitData_.xLast()) break;

		// Evaluate the function
		func = fitEquation(x, params[0], params[1], params[2], params[3], params[4], params[5]);
		delta = fitData_.interpolated(x) - func;
		sos += delta*delta;
		++nPoints;

		// Add on penalty for params[0] (the x intercept) straying too far from the starting value
		delta = fabs(xCentreStart_ - params[0]) - xCentreDeltaLimit_;
		if (delta > 0.0) sos += (delta * 10000.0)*(delta * 10000.0);

		x += windowDelta;
	}

	return sos;
}

// Two-exponential, 5-parameter cost function for modifyBondTerms() fitting
double RefineModule::costFunction2Exp(double params[], int n)
{
	// Copy parameters, adding in zero for AC
	double x[6];
	x[0] = params[0];
	x[1] = params[1];
	x[2] = params[2];
	x[3] = params[3];
	x[4] = 0.0;
	x[5] = params[4];

	return costFunction3Exp(x, 6);
}

// Sum fitting equation with the specified parameters into the specified XYData
void RefineModule::sumFitEquation(XYData& target, double xCentre, double delta, double width, double AL, double AC, double AR)
{
	for (int n=0; n<target.nPoints(); ++n) target.addY(n, fitEquation(target.x(n), xCentre, delta, width, AL, AC, AR));
}
