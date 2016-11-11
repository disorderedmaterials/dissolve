/*
	*** Pair Potential Definition
	*** src/classes/pairpotential.cpp
	Copyright T. Youngs 2012-2016

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

#include "classes/pairpotential.h"
#include "classes/atomtype.h"
#include "base/messenger.h"
#include "base/parameters.h"
#include "math/constants.h"
#include "base/processpool.h"
#include "templates/simplex.h"
#include <math.h>
#include <string.h>

// Constructor
PairPotential::PairPotential() : ListItem<PairPotential>()
{
	sigmaIJ_ = 0.0;
	epsilonIJ_ = 0.0;
	chargeI_ = 0.0;
	chargeJ_ = 0.0;
	nPoints_ = 0;
	delta_ = -1.0;
	type_ = DispersionType;
	rangeSquared_ = 0.0;
}

// Input File Block Keywords
const char* PairPotentialTypeKeywords[] = { "Coulomb", "Dispersion", "Full" };

// Convert text string to PairPotentialType
PairPotential::PairPotentialType PairPotential::pairPotentialType(const char* s)
{
	for (int n=0; n<PairPotential::nPairPotentialTypes; ++n) if (strcmp(s,PairPotentialTypeKeywords[n]) == 0) return (PairPotential::PairPotentialType) n;
	return PairPotential::nPairPotentialTypes;
}

// Convert PairPotentialType to text string
const char* PairPotential::pairPotentialType(PairPotential::PairPotentialType id)
{
	return PairPotentialTypeKeywords[id];
}

/*
 * Potential Type
 */

// Set PairPotential type
void PairPotential::setType(PairPotential::PairPotentialType type)
{
	type_ = type;
}

// Return PairPotential type
PairPotential::PairPotentialType PairPotential::type() const
{
	return type_;
}

/*
 * Source Parameters
 */

// Set Data2D names from source AtomTypes
void PairPotential::setData2DNames()
{
	// Check for NULL pointers
	if (atomTypeI_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL AtomType pointer (atomTypeI_) found in PairPotential::setData2DNames().\n");
		return;
	}
	if (atomTypeJ_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL AtomType pointer (atomTypeJ_) found in PairPotential::setData2DNames().\n");
		return;
	}

	Dnchar s(-1, "%s-%s", atomTypeI_->name(), atomTypeJ_->name()), otherName;
	u_.setName(s);
	otherName.sprintf("%s (Ptb.)", s.get());
	v_.setName(otherName);
	otherName.sprintf("%s (Orig.)", s.get());
	originalU_.setName(otherName);
	otherName.sprintf("%s (dU/dX)", s.get());
	dU_.setName(otherName);
}

// Set source parameters from AtomTypes
void PairPotential::setParameters(AtomType* typeI, AtomType* typeJ)
{
	// Check for NULL pointers
	if (typeI == NULL)
	{
		Messenger::error("NULL_POINTER - NULL AtomType pointer (typeI) given to PairPotential::setParameters().\n");
		return;
	}
	if (typeJ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL AtomType pointer (typeJ) given to PairPotential::setParameters().\n");
		return;
	}
	
	atomTypeI_ = typeI;
	atomTypeJ_ = typeJ;
	setData2DNames();
	Parameters* paramsI = atomTypeI_->parameters();
	Parameters* paramsJ = atomTypeJ_->parameters();
	if ((paramsI == NULL) || (paramsJ == NULL))
	{
		Messenger::error("NULL_POINTER - One or both AtomTypes ('%s' and '%s') given to PairPotential::setParameters() contain a NULL Parameters pointer.\n", atomTypeI_->name(), atomTypeJ_->name());
		sigmaIJ_ = 0.0;
		epsilonIJ_ = 0.0;
		chargeI_ = 0.0;
		chargeJ_ = 0.0;
	}
	else
	{
		// Combine parameters (Lorentz-Berthelot)
		sigmaIJ_ = (paramsI->sigma() + paramsJ->sigma())*0.5;
		epsilonIJ_ = sqrt(paramsI->epsilon() * paramsJ->epsilon());
		chargeI_ = paramsI->charge();
		chargeJ_ = paramsJ->charge();
	}
}

// Set source AtomType pointers only
void PairPotential::setAtomTypes(AtomType* typeI, AtomType* typeJ)
{
	// Check for NULL pointers
	if (typeI == NULL)
	{
		Messenger::error("NULL_POINTER - NULL AtomType pointer (typeI) given to PairPotential::setAtomTypes().\n");
		return;
	}
	if (typeJ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL AtomType pointer (typeJ) given to PairPotential::setAtomTypes().\n");
		return;
	}

	atomTypeI_ = typeI;
	atomTypeJ_ = typeJ;
	setData2DNames();
}

// Return first AtomType name
const char* PairPotential::atomTypeNameI() const
{
	// Check for NULL pointers
	if (atomTypeI_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL AtomType pointer found in PairPotential::atomTypeNameI().\n");
		return "NULL";
	}
	return atomTypeI_->name();
}

// Return second AtomType name
const char* PairPotential::atomTypeNameJ() const
{
	// Check for NULL pointers
	if (atomTypeJ_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL AtomType pointer found in PairPotential::atomTypeNameJ().\n");
		return "NULL";
	}
	return atomTypeJ_->name();
}

// Return first source AtomType
AtomType* PairPotential::atomTypeI() const
{
	return atomTypeI_;
}

// Return second source AtomType
AtomType* PairPotential::atomTypeJ() const
{
	return atomTypeJ_;
}

// Set sigma
void PairPotential::setSigmaIJ(double value)
{
	sigmaIJ_ = value;
}

// Return sigma
double PairPotential::sigmaIJ() const
{
	return sigmaIJ_;
}

// Set epsilon
void PairPotential::setEpsilonIJ(double value)
{
	epsilonIJ_ = value;
}

// Return epsilon
double PairPotential::epsilonIJ() const
{
	return epsilonIJ_;
}

// Set charge I
void PairPotential::setChargeI(double value)
{
	chargeI_ = value;
}

// Return charge I
double PairPotential::chargeI() const
{
	return chargeI_;
}

// Set charge J
void PairPotential::setChargeJ(double value)
{
	chargeJ_ = value;
}

// Return charge J
double PairPotential::chargeJ() const
{
	return chargeJ_;
}

/*
 * Tabulated PairPotential
 */

// Regenerate derivative data
void PairPotential::calculateDerivative()
{
	if (nPoints_ < 2) return;

	double x1, x2;
	for (int n=1; n<nPoints_-1; ++n)
	{
		x1 = u_.x(n-1);
		x2 = u_.x(n+1);
		dU_.setY(n, -(u_.y(n-1) - u_.y(n+1)) / (x2-x1));
	}
	
	// Set first and last points
	dU_.setY(0, 10.0*dU_.y(1));
	dU_.setY(nPoints_-1, 0.0);
}

// Generate potential
bool PairPotential::generate(double maxR, double truncationWidth, double delta)
{
	// Check that AtomType pointers were set at some pointer
	if ((atomTypeI_ == NULL) || (atomTypeJ_ == NULL))
	{
		Messenger::error("NULL_POINTER - One or both AtomTypes in PairPotential are NULL.\n");
		return false;
	}

	// Determine nPoints_
	delta_ = delta;
	rDelta_ = 1.0/delta_;
	rangeSquared_ = maxR*maxR;
	nPoints_ = rangeSquared_ / delta_;
	
	// Initialise arrays
	u_.initialise(nPoints_);
	dU_.initialise(nPoints_);
	
	// Generate potential
	double r, sigmar, sigmar6, sigmar12, truncr, rSq, energy;
	for (int n=1; n<nPoints_; ++n)
	{
		// Calculate r-squared, and the corresponding r, and truncation r
		rSq = n*delta_;
		u_.setX(n, rSq);
		dU_.setX(n, rSq);
		r = sqrt(rSq);

		// Construct potential
		u_.setY(n, 0.0);
		
		// -- Add LJ contribution
		if (type_ != PairPotential::CoulombType)
		{
			sigmar = sigmaIJ_ / r;
			sigmar6 = pow(sigmar, 6.0);
			sigmar12 = sigmar6*sigmar6;
			energy = 4.0 * epsilonIJ_ * ( sigmar12 - sigmar6 );
			
			// Are we into the truncation strip?
			truncr = r - (maxR-truncationWidth);
			if (truncr >= 0)
			{
				// Simple truncation scheme - (cos(x)+1)*0.5, mapping the truncation region to {0,Pi}
				energy *= (cos(PI*(truncr/truncationWidth))+1)*0.5;
			}
			
			// LJ potential only in the current case - no coulomb interaction
			u_.addY(n, energy);
		}
		
		// -- Add Coulomb contribution
		if (type_ != PairPotential::DispersionType)
		{
			energy = COULCONVERT * chargeI_ * chargeJ_ / r;

			// Apply potential truncation
			energy *= (1.0/r - 1.0/maxR);
			u_.addY(n, energy);
		}
	}

	// Since the first point (at zero) would be a nan, set it to ten times the second point instead
	u_.setY(0, 10.0*u_.y(1));

	// Store copy of this original data
	originalU_ = u_;

	// Set up empty array for perturbation potential
	v_ = u_;
	v_.arrayY() = 0.0;

	// Generate derivative data
	calculateDerivative();

	return true;
}

// Return potential at specified r-squared
double PairPotential::energyAtRSquared(double distanceSq) const
{
	// Perform some checks
#ifdef CHECKS
	if (int(distanceSq*rDelta_) < 0)
	{
		Messenger::print("BAD_VALUE - Bin value of rSq is negative (%i) in PairPotential::energyAtRSquared.\n", int(distanceSq*rDelta_));
		return 0.0;
	}
#endif
	// Is distance out of range?
	if (distanceSq > rangeSquared_) return 0.0;
// 	// Determine potential bin
// 	int bin = distanceSq*rDelta_;
// 	return (bin >= nPoints_ ? 0.0 : u_.y(bin));
	return u_.y(distanceSq*rDelta_);
}

// Return derivative at specified r-squared
double PairPotential::forceAtRSquared(double distanceSq) const
{
	// Perform some checks
#ifdef CHECKS
	if (int(distanceSq*rDelta_) < 0)
	{
		Messenger::print("BAD_VALUE - Bin value of rSq is negative (%i) in PairPotential::forceAtRSquared.\n", int(distanceSq*rDelta_));
		return 0.0;
	}
#endif
	// Determine potential bin
	int bin = distanceSq*rDelta_;
	return (bin >= nPoints_ ? 0.0 : dU_.y(bin));
}

// Return tabulated potential
Data2D& PairPotential::u()
{
	return u_;
}

// Return tabulated derivative
Data2D& PairPotential::dU()
{
	return dU_;
}

// Return original potential (without modifications)
Data2D& PairPotential::originalU()
{
	return originalU_;
}

/*
 * Perturbation
 */

// Update perturbation to potential, recreating tabulated data
bool PairPotential::updatePerturbation(Data2D& perturbation, double yScale, double blendFactor)
{
	// Add new perturbation 
// 	v_.addInterpolated(addV);
	v_ = perturbation;
	v_.save("v.txt");
	
// 	// Find first negative value of potential...
// 	ljFitMaximum_ = 7.0;
// 	for (int n=0; n<v_.nPoints(); ++n)
// 	{
// 		ljFitMinimum_ = v_.x(n);
// 		if (v_.y(n) < 0.0) break;
// 	}
// 	Messenger::print("LJ fit range is %f to %f Angstroms.\n", ljFitMinimum_, ljFitMaximum_);
// 	
// 	// Fit LJ potential to first minimum well in perturbation
// 	Simplex<PairPotential> ljFitSimplex(this, &PairPotential::potentialFitCost);
// 	Array<double> alpha;
// 	alpha.add(3.0);	// Sigma
// 	alpha.add(0.1);	// Epsilon
// 	alpha.add(12.0);// Power 1
// 	alpha.add(6.0);	// Power 2
// 
// 	ljFitSimplex.initialise(alpha, 0.1);
// 	alpha = ljFitSimplex.minimise(100, 100, 1.0e-5, 1.0);
// 	for (int n=0; n< alpha.nItems(); ++n) printf("%i  %f\n", n, alpha[n]);
// 	printf("Final cost is %f\n", potentialFitCost(alpha));
// 	double lj, r, sigma = alpha[0], epsilon = alpha[1], power1 = alpha[2], power2 = alpha[3];
	double r;
	for (int n=1; n<u_.nPoints(); ++n)
	{
		r = sqrt(u_.x(n));
// 		lj = yScale * 4.0*epsilon * (pow(sigma/r, power1) - pow(sigma/r, power2));

		// Blend LJ potential with remainder of function
// 		u_.arrayY()[n] += yScale * (lj + (v_.interpolated(r) - lj) * blendFactor);
		
		u_.arrayY()[n] -= v_.interpolated(r) * 0.1;
	}
	u_.save("pp.txt");

	// Modify pair potential
// 	u_ = originalU_;
// 	for (int n=0; n<u_.nPoints(); ++n) u_.arrayY()[n] += v_.interpolated(sqrt(u_.x(n)));
// 	for (int n=0; n<u_.nPoints(); ++n) u_.arrayY()[n] = v_.interpolated(sqrt(u_.x(n)));
	calculateDerivative();

	return true;
}

// Cost function callback (passed to Simplex on construction)
double PairPotential::potentialFitCost(Array<double>& alpha)
{
	double r, cost = 0.0;
	double lj, sigma = alpha[0], epsilon = alpha[1], power1 = alpha[2], power2 = alpha[3];
	for (int n=0; n<v_.nPoints(); ++n)
	{
		r = v_.x(n);
		if (r < ljFitMinimum_) continue;
		if (r > ljFitMaximum_) break;
		lj = 4.0*epsilon * (pow(sigma/r, power1) - pow(sigma/r, power2));
		cost += (v_.y(n) - lj) * (v_.y(n) - lj) * (ljFitMinimum_ / r) * (ljFitMinimum_ / r);
	}
	return cost;
}

// Clear perturbation to potential, reverting to original
void PairPotential::clearPerturbation()
{
	u_ = originalU_;
	calculateDerivative();
}

// Return modification to PairPotential
Data2D& PairPotential::v()
{
	return v_;
}

/*
 * File I/O
 */

// Save PairPotential data to specified file
bool PairPotential::save(const char* filename)
{
	// Open file and check that we're OK to proceed writing to it
	LineParser parser;
	Messenger::print("Writing PairPotential file '%s'...\n", filename);

	parser.openOutput(filename, true);
	if (!parser.isFileGoodForWriting())
	{
		Messenger::error("Couldn't open file '%s' for writing.\n", filename);
		return false;
	}
	
	parser.writeLineF("#%9s  %12s  %12s  %12s  %12s\n", "r(Angs)", "U(kJ/mol)", "dU(kJ/mol/Ang)", "U0(kJ/mol)", "V(kJ/mol)");
	if (v_.nPoints() > 0) for (int n = 0; n<nPoints_; ++n) parser.writeLineF("%10.4e  %12.4e  %12.4e  %12.4e  %12.4e\n", sqrt(u_.x(n)), u_.y(n), dU_.y(n), originalU_.y(n), v_.interpolated(sqrt(u_.x(n))));
	else for (int n = 0; n<nPoints_; ++n) parser.writeLineF("%10.4e  %12.4e  %12.4e  %12.4e  %12.4e\n", sqrt(u_.x(n)), u_.y(n), dU_.y(n), originalU_.y(n), 0.0);
	parser.closeFiles();
	return true;
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool PairPotential::broadcast(ProcessPool& procPool, const List<AtomType>& atomTypes)
{
#ifdef PARALLEL
	// PairPotential type
	int n = type_;
	procPool.broadcast(&n, 1);
	type_ = (PairPotential::PairPotentialType) n;

	// Source Parameters - Master needs to determine AtomType indices
	int index;
	if (procPool.isMaster()) index = atomTypes.indexOf(atomTypeI_);
	if (!procPool.broadcast(&index, 1)) return false;
	atomTypeI_ = atomTypes.item(index);
	if (procPool.isMaster()) index = atomTypes.indexOf(atomTypeJ_);
	if (!procPool.broadcast(&index, 1)) return false;
	atomTypeJ_ = atomTypes.item(index);
	if (!procPool.broadcast(&sigmaIJ_, 1)) return false;
	if (!procPool.broadcast(&epsilonIJ_, 1)) return false;
	if (!procPool.broadcast(&chargeI_, 1)) return false;
	if (!procPool.broadcast(&chargeJ_, 1)) return false;

	// Tabulated Potential
	if (!procPool.broadcast(&nPoints_, 1)) return false;
	if (!procPool.broadcast(&delta_, 1)) return false;
	if (!procPool.broadcast(&rDelta_, 1)) return false;
	u_.broadcast(procPool);
	originalU_.broadcast(procPool);
	dU_.broadcast(procPool);
#endif
	return true;
}
