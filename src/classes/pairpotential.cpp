/*
	*** Interatomic Pair Potential
	*** src/classes/pairpotential.cpp
	Copyright T. Youngs 2012-2017

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
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "math/constants.h"
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
	range_ = 0.0;
	rangeSquared_ = 0.0;
	includeCharges_ = true;
	shortRangeType_ = PairPotential::LennardJonesType;
	coulombTruncationScheme_ = PairPotential::ShiftedTruncation;
	rangeSquared_ = 0.0;
}

// Short-range typeKeywords
const char* ShortRangeTypeKeywords[] = { "None", "LJ" };

// Convert text string to ShortRangeType
PairPotential::ShortRangeType PairPotential::shortRangeType(const char* s)
{
	for (int n=0; n<PairPotential::nShortRangeTypes; ++n) if (DUQSys::sameString(s,ShortRangeTypeKeywords[n])) return (PairPotential::ShortRangeType) n;
	return PairPotential::nShortRangeTypes;
}

// Convert ShortRangeType to text string
const char* PairPotential::shortRangeType(PairPotential::ShortRangeType id)
{
	return ShortRangeTypeKeywords[id];
}

// Truncation Scheme Keywords
const char* TruncationSchemeKeywords[] = { "None", "Shifted" };

// Convert text string to TruncationScheme
PairPotential::TruncationScheme PairPotential::truncationScheme(const char* s)
{
	for (int n=0; n<PairPotential::nTruncationSchemes; ++n) if (DUQSys::sameString(s,TruncationSchemeKeywords[n])) return (PairPotential::TruncationScheme) n;
	return PairPotential::nTruncationSchemes;
}

// Convert TruncationScheme to text string
const char* PairPotential::truncationScheme(TruncationScheme id)
{
	return TruncationSchemeKeywords[id];
}

/*
 * Potential Type
 */

// Set short-ranged type
void PairPotential::setShortRangeType(PairPotential::ShortRangeType type)
{
	shortRangeType_ = type;
}

// Return short-ranged type
PairPotential::ShortRangeType PairPotential::shortRangeType() const
{
	return shortRangeType_;
}

// Set Coulomb truncation scheme
void PairPotential::setCoulombTruncationScheme(PairPotential::TruncationScheme scheme)
{
	coulombTruncationScheme_ = scheme;
}

// Return Coulomb truncation scheme
PairPotential::TruncationScheme PairPotential::coulombTruncationScheme() const
{
	return coulombTruncationScheme_;
}

/*
 * Source Parameters
 */

// Set XYData names from source AtomTypes
void PairPotential::setXYDataNames()
{
	// Check for NULL pointers
	if (atomTypeI_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL AtomType pointer (atomTypeI_) found in PairPotential::setXYDataNames().\n");
		return;
	}
	if (atomTypeJ_ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL AtomType pointer (atomTypeJ_) found in PairPotential::setXYDataNames().\n");
		return;
	}

	CharString s("%s-%s", atomTypeI_->name(), atomTypeJ_->name()), otherName;
	uFull_.setName(s);
	otherName.sprintf("%s (Add)", s.get());
	uAdditional_.setName(otherName);
	otherName.sprintf("%s (Orig)", s.get());
	uOriginal_.setName(otherName);
	otherName.sprintf("%s (dU/dr)", s.get());
	dUFull_.setName(otherName);
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
	setXYDataNames();
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
	setXYDataNames();
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

// Calculate full potential
void PairPotential::calculateUFull()
{
	// Copy uOriginal_ into uFull_...
	uFull_ = uOriginal_;

	// ...add on uAdditional...
	uFull_ += uAdditional_;

	// ...and interpolate it
	uFull_.interpolate(XYData::ThreePointInterpolation);
}

// Return analytic short range potential energy
double PairPotential::analyticShortRangeEnergy(double r, PairPotential::ShortRangeType type)
{
	// Standard Lennard-Jones potential
	if (type == PairPotential::LennardJonesType)
	{
		double sigmar = sigmaIJ_ / r;
		double sigmar6 = pow(sigmar, 6.0);
		double sigmar12 = sigmar6*sigmar6;
		double energy = 4.0 * epsilonIJ_ * ( sigmar12 - sigmar6 );
		
		// Are we into the truncation strip?
		double truncr = r - (range_-truncationWidth_);
		if (truncr >= 0)
		{
			// Simple truncation scheme - (cos(x)+1)*0.5, mapping the truncation region to {0,Pi}
			energy *= (cos(PI*(truncr/truncationWidth_))+1)*0.5;
		}

		return energy;
	}

	return 0.0;
}

// Return analytic coulomb potential energy
double PairPotential::analyticCoulombEnergy(double r)
{
	// Calculate energy including truncation scheme (truncated and shifted sum)
	double energy = 0.0;

	// Calculate based on truncation scheme
	if (coulombTruncationScheme_ == PairPotential::NoTruncation) energy = COULCONVERT * chargeI_ * chargeJ_ / r;
	else if (coulombTruncationScheme_ == PairPotential::ShiftedTruncation) energy = COULCONVERT * chargeI_ * chargeJ_ * (1.0/r + r/rangeSquared_ - 2.0/range_);

	return energy;
}

// Return analytic short range force
double PairPotential::analyticShortRangeForce(double r, PairPotential::ShortRangeType type)
{
	// Standard Lennard-Jones potential
	if (type == PairPotential::LennardJonesType)
	{
		// f = -48*epsilon*((sigma**12/x**13)-0.5*(sigma**6/x**7))

		double sigmar = sigmaIJ_ / r;
		double sigmar6 = pow(sigmar, 6.0);
		double sigmar12 = sigmar6*sigmar6;
		double force;

		// Are we into the truncation strip?
		double truncr = r - (range_-truncationWidth_);
		if (truncr >= 0)
		{
			// Simple truncation scheme - (cos(x)+1)*0.5, mapping the truncation region to {0,Pi}
			// d/dx = -PI*sin((PI*truncr)/truncationWidth_) / truncationWidth_
			double de_t = (-48.0 * epsilonIJ_ * ( (sigmar12 / pow(r, 13.0)) - 0.5 * (sigmar6 / pow(r, 7.0)) )) * (cos(PI*(truncr/truncationWidth_))+1)*0.5;
			double e_dt = (4.0 * epsilonIJ_ * ( sigmar12 - sigmar6 )) * -PI*sin((PI*truncr)/truncationWidth_) / truncationWidth_;
			force = de_t * e_dt;
		}
		else force = 48.0*epsilonIJ_ * sigmar6 * (-sigmar6 + 0.5) / r;

		return force;
	}

	return 0.0;
}

// Return analytic coulomb potential energy
double PairPotential::analyticCoulombForce(double r)
{
	// Calculate energy including truncation scheme (truncated and shifted sum)
	double force = 0.0;

	// Calculate based on truncation scheme
	if (coulombTruncationScheme_ == PairPotential::NoTruncation) force = -COULCONVERT * chargeI_ * chargeJ_ / (r*r);
	else if (coulombTruncationScheme_ == PairPotential::ShiftedTruncation) force = -COULCONVERT * chargeI_ * chargeJ_ * (1.0/(r*r) - 1.0/rangeSquared_);

	return force;
}

// Regenerate derivative data
void PairPotential::calculateDUFull()
{
	dUFull_.templateFrom(uFull_);

	if (nPoints_ < 2) return;

	double x1, x2;
	for (int n=1; n<nPoints_-1; ++n)
	{
		x1 = sqrt(uFull_.x(n-1));
		x2 = sqrt(uFull_.x(n+1));
		dUFull_.setY(n, -(uFull_.y(n-1) - uFull_.y(n+1)) / (x2-x1));
	}
	
	// Set first and last points
	dUFull_.setY(0, 10.0*dUFull_.y(1));
	dUFull_.setY(nPoints_-1, 0.0);

	// Interpolate it
	dUFull_.interpolate(XYData::ThreePointInterpolation);
}

// Setup and generate initial potential
bool PairPotential::setup(double maxR, double truncationWidth, double delta, bool includeCharges)
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
	range_ = maxR;
	rangeSquared_ = maxR*maxR;
	truncationWidth_ = truncationWidth;
	includeCharges_ = includeCharges;
	nPoints_ = rangeSquared_ / delta_;

	// Initialise original and additional potential arrays, and calculate original potential
	uOriginal_.initialise(nPoints_);
	calculateUOriginal(false);

	// Set additional potential to zero (or load it - TODO) and update full potential
	uAdditional_ = uOriginal_;
	uAdditional_.arrayY() = 0.0;
	calculateUFull();

	// Generate derivative data
	dUFull_.initialise(nPoints_);
	calculateDUFull();

	return true;
}

// (Re)generate potential from current parameters
bool PairPotential::calculateUOriginal(bool recalculateUFull)
{
	double r, sigmar, sigmar6, sigmar12, truncr, rSq, energy;

	for (int n=1; n<nPoints_; ++n)
	{
		// Calculate r-squared, and the corresponding r, and truncation r
		rSq = n*delta_;
		uOriginal_.setX(n, rSq);
		r = sqrt(rSq);

		// Construct potential
		uOriginal_.setY(n, 0.0);

		// Short-range potential contribution
		uOriginal_.addY(n, analyticShortRangeEnergy(r, shortRangeType_));
		
		// -- Add Coulomb contribution
		if (includeCharges_) uOriginal_.addY(n, analyticCoulombEnergy(r));
	}

	// Since the first point (at zero) risks being a nan, set it to ten times the second point instead
	uOriginal_.setY(0, 10.0*uOriginal_.y(1));

	// Create a spline interpolation of the original potential
	uOriginal_.interpolate(XYData::ThreePointInterpolation);

	// Update full potential (if not the first generation of the potential)
	if (recalculateUFull) calculateUFull();
}

// Return potential at specified r-squared
double PairPotential::energyAtRSquared(double distanceSq)
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
// 	if (distanceSq >= rangeSquared_) return 0.0;

	// Return binned value
// 	return uFull_.y(distanceSq*rDelta_);

	// Return interpolated value
	return uFull_.interpolated(distanceSq, distanceSq*rDelta_);
}

// Return analytic potential at specified r-squared
double PairPotential::analyticEnergyAtRSquared(double rSq)
{
	double energy = 0.0;
	double r = sqrt(rSq);

	if (rSq > rangeSquared_) return 0.0;

	// Short-range potential
	energy += analyticShortRangeEnergy(r, shortRangeType_);

	// Coulomb contribution
	if (includeCharges_) energy += analyticCoulombEnergy(r);

	return energy;
}

// Return derivative at specified r-squared
double PairPotential::forceAtRSquared(double distanceSq)
{
	// Perform some checks
#ifdef CHECKS
	if (int(distanceSq*rDelta_) < 0)
	{
		Messenger::print("BAD_VALUE - Bin value of rSq is negative (%i) in PairPotential::forceAtRSquared.\n", int(distanceSq*rDelta_));
		return 0.0;
	}
#endif
	// Return binned value
// 	int bin = distanceSq*rDelta_;
// 	return (bin >= nPoints_ ? 0.0 : dUFull_.y(bin));

	// Return interpolated value
	return dUFull_.interpolated(distanceSq, distanceSq*rDelta_);
}

// Return analytic derivative of potential at specified r-squared
double PairPotential::analyticForceAtRSquared(double rSq)
{
	if (rSq > rangeSquared_) return 0.0;

	double force = 0.0;
	double r = sqrt(rSq);

	// Short-range potential
	force += analyticShortRangeForce(r, shortRangeType_);

	// Coulomb contribution
	if (includeCharges_) force += analyticCoulombForce(r);

	return force;
}

// Return full tabulated potential (original plus additional)
XYData& PairPotential::uFull()
{
	return uFull_;
}

// Return full tabulated derivative
XYData& PairPotential::dUFull()
{
	return dUFull_;
}

// Return original potential (calculated from AtomType parameters)
XYData& PairPotential::uOriginal()
{
	return uOriginal_;
}

// Return additional potential (generated by some means)
XYData& PairPotential::uAdditional()
{
	return uAdditional_;
}

// Zero additional potential
void PairPotential::resetUAdditional()
{
	uAdditional_.arrayY() = 0.0;

	calculateUFull();
	calculateDUFull();
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
	
	parser.writeLineF("#%9s  %12s  %12s  %12s  %12s  %12s  %12s\n", "", "Full", "Derivative", "Original", "Additional", "Exact(Orig)", "Exact(Deriv)");
	parser.writeLineF("#%9s  %12s  %12s  %12s  %12s  %12s  %12s\n", "r(Angs)", "U(kJ/mol)", "dU(kJ/mol/Ang)", "U(kJ/mol)", "U(kJ/mol)", "U(kJ/mol)", "dU(kJ/mol/Ang)");
	for (int n = 0; n<nPoints_; ++n) parser.writeLineF("%10.6e  %12.6e  %12.6e  %12.6e  %12.6e  %12.6e  %12.6e\n", sqrt(uOriginal_.x(n)), uFull_.y(n), dUFull_.y(n), uOriginal_.y(n), uAdditional_.interpolated(sqrt(uOriginal_.x(n))), analyticEnergyAtRSquared(uOriginal_.x(n)), analyticForceAtRSquared(uOriginal_.x(n)));

	parser.closeFiles();

	CharString test("%s.test", filename);
	LineParser shitParser;
	shitParser.openOutput(test);
	double x = 0.0, delta = delta_*0.05;
	while (x < rangeSquared_)
	{
		shitParser.writeLineF("%12.6e  %12.6e  %12.6e  %12.6e\n", sqrt(x), uFull_.interpolated(x), analyticEnergyAtRSquared(x), (uFull_.interpolated(x) - analyticEnergyAtRSquared(x))*100.0);
		x += delta;
	}
	shitParser.closeFiles();
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
	if (!procPool.broadcast(EnumCast<PairPotential::ShortRangeType>(shortRangeType_))) return false;
	if (!procPool.broadcast(includeCharges_)) return false;

	// Source Parameters - Master needs to determine AtomType indices
	int index;
	if (procPool.isMaster()) index = atomTypes.indexOf(atomTypeI_);
	if (!procPool.broadcast(index)) return false;
	atomTypeI_ = atomTypes.item(index);
	if (procPool.isMaster()) index = atomTypes.indexOf(atomTypeJ_);
	if (!procPool.broadcast(index)) return false;
	atomTypeJ_ = atomTypes.item(index);
	if (!procPool.broadcast(sigmaIJ_)) return false;
	if (!procPool.broadcast(epsilonIJ_)) return false;
	if (!procPool.broadcast(chargeI_)) return false;
	if (!procPool.broadcast(chargeJ_)) return false;

	// Tabulation Parameters
	if (!procPool.broadcast(range_)) return false;
	if (!procPool.broadcast(rangeSquared_)) return false;
	if (!procPool.broadcast(truncationWidth_)) return false;
	if (!procPool.broadcast(nPoints_)) return false;
	if (!procPool.broadcast(delta_)) return false;
	if (!procPool.broadcast(rDelta_)) return false;

	// Tabulations
	uOriginal_.broadcast(procPool);
	uAdditional_.broadcast(procPool);
	uFull_.broadcast(procPool);
	dUFull_.broadcast(procPool);
#endif
	return true;
}
