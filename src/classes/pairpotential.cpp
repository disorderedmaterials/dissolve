/*
	*** Interatomic Pair Potential
	*** src/classes/pairpotential.cpp
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

#include "classes/pairpotential.h"
#include "classes/atomtype.h"
#include "base/messenger.h"
#include "base/lineparser.h"
#include "base/parameters.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "math/constants.h"
#include "templates/enumhelpers.h"
#include <math.h>
#include <string.h>

// Static members
PairPotential::CoulombTruncationScheme PairPotential::coulombTruncationScheme_ = PairPotential::ShiftedCoulombTruncation;
PairPotential::ShortRangeTruncationScheme PairPotential::shortRangeTruncationScheme_ = PairPotential::ShiftedShortRangeTruncation;
double PairPotential::shortRangeTruncationWidth_ = 2.0;

// Constructor
PairPotential::PairPotential() : ListItem<PairPotential>()
{
	for (int n=0; n<MAXSRPARAMETERS; ++n) parameters_[n] = 0.0;
	chargeI_ = 0.0;
	chargeJ_ = 0.0;
	nPoints_ = 0;
	delta_ = -1.0;
	range_ = 0.0;
	includeCoulomb_ = true;
	shortRangeType_ = PairPotential::LennardJonesType;
}

// Short-range typeKeywords
const char* ShortRangeTypeKeywords[] = { "None", "LJ", "LJGeometric" };

// Convert text string to ShortRangeType
PairPotential::ShortRangeType PairPotential::shortRangeType(const char* s)
{
	for (int n=0; n<PairPotential::nShortRangeTypes; ++n) if (DissolveSys::sameString(s,ShortRangeTypeKeywords[n])) return (PairPotential::ShortRangeType) n;
	return PairPotential::nShortRangeTypes;
}

// Convert ShortRangeType to text string
const char* PairPotential::shortRangeType(PairPotential::ShortRangeType id)
{
	return ShortRangeTypeKeywords[id];
}

// Return ShortRangeTypes array
const char** PairPotential::shortRangeTypes()
{
	return ShortRangeTypeKeywords;
}

// Coulomb Truncation Scheme Keywords
const char* CoulombTruncationSchemeKeywords[] = { "None", "Shifted" };

// Convert text string to TruncationScheme
PairPotential::CoulombTruncationScheme PairPotential::coulombTruncationScheme(const char* s)
{
	for (int n=0; n<PairPotential::nCoulombTruncationSchemes; ++n) if (DissolveSys::sameString(s,CoulombTruncationSchemeKeywords[n])) return (PairPotential::CoulombTruncationScheme) n;
	return PairPotential::nCoulombTruncationSchemes;
}

// Convert CoulombTruncationScheme to text string
const char* PairPotential::coulombTruncationScheme(CoulombTruncationScheme id)
{
	return CoulombTruncationSchemeKeywords[id];
}

// Return CoulombTruncationScheme array
const char** PairPotential::coulombTruncationSchemes()
{
	return CoulombTruncationSchemeKeywords;
}

// Short-Range Truncation Scheme Keywords
const char* ShortRangeTruncationSchemeKeywords[] = { "None", "Shifted", "Cosine" };

// Convert text string to ShortRangeTruncationScheme
PairPotential::ShortRangeTruncationScheme PairPotential::shortRangeTruncationScheme(const char* s)
{
	for (int n=0; n<PairPotential::nShortRangeTruncationSchemes; ++n) if (DissolveSys::sameString(s,ShortRangeTruncationSchemeKeywords[n])) return (PairPotential::ShortRangeTruncationScheme) n;
	return PairPotential::nShortRangeTruncationSchemes;
}

// Convert ShortRangeTruncationScheme to text string
const char* PairPotential::shortRangeTruncationScheme(ShortRangeTruncationScheme id)
{
	return ShortRangeTruncationSchemeKeywords[id];
}

// Return ShortRangeTruncationScheme array
const char** PairPotential::shortRangeTruncationSchemes()
{
	return ShortRangeTruncationSchemeKeywords;
}

/*
 * Seed Interaction Type
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

// Set short-ranged truncation scheme
void PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme scheme)
{
	shortRangeTruncationScheme_ = scheme;
}

// Return short-ranged truncation scheme
PairPotential::ShortRangeTruncationScheme PairPotential::shortRangeTruncationScheme()
{
	return shortRangeTruncationScheme_;
}

// Set width of short-range potential over which to truncate (if scheme = Cosine)
void PairPotential::setShortRangeTruncationWidth(double width)
{
	shortRangeTruncationWidth_ = width;
}

// Return width of short-range potential over which to truncate (if scheme = Cosine)
double PairPotential::shortRangeTruncationWidth()
{
	return shortRangeTruncationWidth_;
}

// Set whether Coulomb term should be included in the generated potential
void PairPotential::setIncludeCoulomb(bool b)
{
	includeCoulomb_ = b;
}

// Return whether Coulomb term should be included in the generated potential
bool PairPotential::includeCoulomb()
{
	return includeCoulomb_;
}

// Set Coulomb truncation scheme
void PairPotential::setCoulombTruncationScheme(PairPotential::CoulombTruncationScheme scheme)
{
	coulombTruncationScheme_ = scheme;
}

// Return Coulomb truncation scheme
PairPotential::CoulombTruncationScheme PairPotential::coulombTruncationScheme()
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

	uFull_.setName(CharString("%s-%s", atomTypeI_->name(), atomTypeJ_->name()));
	uFull_.setObjectName(CharString("PairPotential//%s-%s//Full", atomTypeI_->name(), atomTypeJ_->name()));

	uAdditional_.setName(CharString("%s-%s (Add)", atomTypeI_->name(), atomTypeJ_->name()));
	uAdditional_.setObjectName(CharString("PairPotential//%s-%s//Additional", atomTypeI_->name(), atomTypeJ_->name()));

	uOriginal_.setName(CharString("%s-%s (Orig)", atomTypeI_->name(), atomTypeJ_->name()));
	uOriginal_.setObjectName(CharString("PairPotential//%s-%s//Original", atomTypeI_->name(), atomTypeJ_->name()));

	dUFull_.setName(CharString("%s-%s (dU/dr)", atomTypeI_->name(), atomTypeJ_->name()));
	dUFull_.setObjectName(CharString("PairPotential//%s-%s//Force", atomTypeI_->name(), atomTypeJ_->name()));
}

// Set source parameters from AtomTypes
bool PairPotential::setParameters(AtomType* typeI, AtomType* typeJ)
{
	// Check for NULL pointers
	if (typeI == NULL)
	{
		Messenger::error("NULL_POINTER - NULL AtomType pointer (typeI) given to PairPotential::setParameters().\n");
		return false;
	}
	if (typeJ == NULL)
	{
		Messenger::error("NULL_POINTER - NULL AtomType pointer (typeJ) given to PairPotential::setParameters().\n");
		return false;
	}
	
	atomTypeI_ = typeI;
	atomTypeJ_ = typeJ;
	setXYDataNames();
	Parameters& paramsI = atomTypeI_->parameters();
	Parameters& paramsJ = atomTypeJ_->parameters();

	// Sanity check - are either of the parameter sets empty (i.e. have never been set with useful data)?
	if (paramsI.empty() || paramsJ.empty())
	{
		if (paramsI.empty() && paramsJ.empty()) Messenger::error("Can't set parameters for PairPotential since neither AtomType (%s and %s) contain any parameters.\n", atomTypeI_->name(), atomTypeJ_->name());
		else if (paramsI.empty()) Messenger::error("Can't set parameters for PairPotential since AtomType %s contains no parameters.\n", atomTypeI_->name());
		else Messenger::error("Can't set parameters for PairPotential since AtomType %s contains no parameters.\n", atomTypeJ_->name());
		return false;
	}

	// Combine / set parameters as necessary, depending on the sr interaction type of this PairPotential
	switch (shortRangeType_)
	{
		case (PairPotential::NoInteractionType):
			break;
		case (PairPotential::LennardJonesType):
			/*
			 * Combine parameters (Lorentz-Berthelot):
			 * Parameter 0 = Epsilon
			 * Parameter 1 = Sigma
			 */
			parameters_[0] = sqrt(paramsI.parameter(0) * paramsJ.parameter(0));
			parameters_[1] = (paramsI.parameter(1) + paramsJ.parameter(1))*0.5;
			chargeI_ = paramsI.charge();
			chargeJ_ = paramsJ.charge();
			break;
		case (PairPotential::LennardJonesGeometricType):
			/*
			 * Combine parameters (Geometric):
			 * Parameter 0 = Epsilon
			 * Parameter 1 = Sigma
			 */
			parameters_[0] = sqrt(paramsI.parameter(0) * paramsJ.parameter(0));
			parameters_[1] = sqrt(paramsI.parameter(1) * paramsJ.parameter(1));
			chargeI_ = paramsI.charge();
			chargeJ_ = paramsJ.charge();
			break;
		default:
			Messenger::error("Short-range type %i is not accounted for in PairPotential::setParameters().\n", shortRangeType_);
			return false;
	}

	return true;
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

// Set parameter with index specified
void PairPotential::setParameter(int index, double value)
{
#ifdef CHECKS
	if ((index < 0) || (index >= MAXSRPARAMETERS))
	{
		Messenger::error("OUT_OF_RANGE - PairPotential Parameter index %i is out of range (MAXSRPARAMETERS = %i) so it cannot be set.\n", index, MAXSRPARAMETERS);
		return;
	}
#endif
	 parameters_[index] = value;
}

// Return short-range parameter with index specified
double PairPotential::parameter(int index) const
{
#ifdef CHECKS
	if ((index < 0) || (index >= MAXSRPARAMETERS))
	{
		Messenger::error("OUT_OF_RANGE - PairPotential Parameter index %i is out of range (MAXSRPARAMETERS = %i) so it cannot be returned.\n", index, MAXSRPARAMETERS);
		return 0.0;
	}
#endif
	 return parameters_[index];
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

// Return analytic short range potential energy
double PairPotential::analyticShortRangeEnergy(double r, PairPotential::ShortRangeType type, PairPotential::ShortRangeTruncationScheme truncation)
{
	if (type == PairPotential::NoInteractionType) return 0.0;
	else if ((type == PairPotential::LennardJonesType) || (type == PairPotential::LennardJonesGeometricType))
	{
		/*
		 * Standard Lennard-Jones potential
		 * Parameter 0 = Epsilon
		 * Parameter 1 = Sigma
		 */
		double sigmar = parameters_[1] / r;
		double sigmar6 = pow(sigmar, 6.0);
		double sigmar12 = sigmar6*sigmar6;
		double energy = 4.0 * parameters_[0] * ( sigmar12 - sigmar6 );
		
		// Apply the selected truncation scheme
		if (truncation == PairPotential::ShiftedShortRangeTruncation)
		{
			energy += -(r - range_)*shortRangeForceAtCutoff_ - shortRangeEnergyAtCutoff_;
		}
		else if (truncation == PairPotential::CosineShortRangeTruncation)
		{
			// Are we into the truncation strip?
			double truncr = r - (range_-shortRangeTruncationWidth_);
			if (truncr >= 0)
			{
				// Simple truncation scheme - (cos(x)+1)*0.5, mapping the truncation region to {0,Pi}
				energy *= (cos(PI*(truncr/shortRangeTruncationWidth_))+1)*0.5;
			}
		}

		return energy;
	}

	Messenger::error("Short-range interaction type %i is not accounted for in PairPotential::analyticShortRangeEnergy(). Returning 0.0...\n", type);

	return 0.0;
}

// Return analytic short range force
double PairPotential::analyticShortRangeForce(double r, PairPotential::ShortRangeType type, PairPotential::ShortRangeTruncationScheme truncation)
{
	if (type == PairPotential::NoInteractionType) return 0.0;
	else if ((type == PairPotential::LennardJonesType) || (type == PairPotential::LennardJonesGeometricType))
	{
		/*
		 * Standard Lennard-Jones potential
		 * Parameter 0 = Epsilon
		 * Parameter 1 = Sigma
		 */

		// f = -48*epsilon*((sigma**12/x**13)-0.5*(sigma**6/x**7))

		double sigmar = parameters_[1] / r;
		double sigmar6 = pow(sigmar, 6.0);
		double sigmar12 = sigmar6*sigmar6;

		// Apply the selected truncation scheme
		if (truncation == PairPotential::NoShortRangeTruncation) return 48.0*parameters_[0] * sigmar6 * (-sigmar6 + 0.5) / r;
		else if (truncation == PairPotential::ShiftedShortRangeTruncation)
		{
			return (48.0*parameters_[0] * sigmar6 * (-sigmar6 + 0.5) / r) - shortRangeForceAtCutoff_;
		}
		else if (truncation == PairPotential::CosineShortRangeTruncation)
		{
			// Are we into the truncation strip?
			double truncr = r - (range_-shortRangeTruncationWidth_);
			if (truncr >= 0)
			{
				// Simple truncation scheme - (cos(x)+1)*0.5, mapping the truncation region to {0,Pi}
				// d/dx = -PI*sin((PI*truncr)/shortRangeTruncationWidth_) / shortRangeTruncationWidth_
				double de_t = (-48.0 * parameters_[0] * ( (sigmar12 / pow(r, 13.0)) - 0.5 * (sigmar6 / pow(r, 7.0)) )) * (cos(PI*(truncr/shortRangeTruncationWidth_))+1)*0.5;
				double e_dt = (4.0 * parameters_[0] * ( sigmar12 - sigmar6 )) * -PI*sin((PI*truncr)/shortRangeTruncationWidth_) / shortRangeTruncationWidth_;
				return de_t * e_dt;
			}
			else return 48.0*parameters_[0] * sigmar6 * (-sigmar6 + 0.5) / r;
		}
	}

	Messenger::error("Short-range interaction type %i is not accounted for in PairPotential::analyticShortRangeForce(). Returning 0.0...\n", type);

	return 0.0;
}

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

// Calculate derivative of potential
void PairPotential::calculateDUFull()
{
	dUFull_.templateFrom(uFull_);

	if (nPoints_ < 2) return;

	double fprime;
	for (int n=1; n<nPoints_-1; ++n)
	{
		/* Calculate numerical derivative with five-point stencil if possible. Otherwise use three-point stencil.
		 * Assumes data are regularly-spaced (they should be, with gap of delta_)
		 *            -f(x+2) + 8 f(x+1) - 8 f(x-1) + f(x-2)
		 *    f'(x) = --------------------------------------
		 *                           12 h 
		 */
		if ((n == 1) || (n == (nPoints_-2)))
		{
			// Three-point 
			dUFull_.setY(n, -(uFull_.y(n-1) - uFull_.y(n+1)) / (2*delta_));
		}
		else
		{
			// Five-point stencil
			fprime = -uFull_.y(n+2) + 8*uFull_.y(n+1) - 8*uFull_.y(n-1) + uFull_.y(n-2);
			fprime /= 12*delta_;
			dUFull_.setY(n, fprime);
		}
	}
	
	// Set first and last points
	dUFull_.setY(0, 10.0*dUFull_.y(1));
	dUFull_.setY(nPoints_-1, dUFull().y(nPoints_-2));

	// Interpolate it
	dUFull_.interpolate(XYData::ThreePointInterpolation);
}

// Set up and generate initial potential
bool PairPotential::setUp(double maxR, double delta, bool includeCoulomb)
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
	includeCoulomb_ = includeCoulomb;
	nPoints_ = range_ / delta_;

	// Calculate energies and forces at the cutoff distance, for later use in truncation schemes
	shortRangeEnergyAtCutoff_ = analyticShortRangeEnergy(range_, shortRangeType_, PairPotential::NoShortRangeTruncation);
	shortRangeForceAtCutoff_ = analyticShortRangeForce(range_, shortRangeType_, PairPotential::NoShortRangeTruncation);
	coulombEnergyAtCutoff_ = analyticCoulombEnergy(chargeI_*chargeJ_, range_, PairPotential::NoCoulombTruncation);
	coulombForceAtCutoff_ = analyticCoulombForce(chargeI_*chargeJ_, range_, PairPotential::NoCoulombTruncation);

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

// Return range of potential
double PairPotential::range() const
{
	return range_;
}

// (Re)generate potential from current parameters
void PairPotential::calculateUOriginal(bool recalculateUFull)
{
	double r;

	for (int n=1; n<nPoints_; ++n)
	{
		r = n*delta_;
		uOriginal_.setX(n, r);

		// Construct potential
		uOriginal_.setY(n, 0.0);

		// Short-range potential contribution
		uOriginal_.addY(n, analyticShortRangeEnergy(r, shortRangeType_));
		
		// -- Add Coulomb contribution
		if (includeCoulomb_) uOriginal_.addY(n, analyticCoulombEnergy(chargeI_*chargeJ_, r));
	}

	// Since the first point (at zero) risks being a nan, set it to ten times the second point instead
	uOriginal_.setY(0, 10.0*uOriginal_.y(1));

	// Create a spline interpolation of the original potential
	uOriginal_.interpolate(XYData::ThreePointInterpolation);

	// Update full potential (if not the first generation of the potential)
	if (recalculateUFull) calculateUFull();
}

// Return potential at specified r
double PairPotential::energy(double r)
{
	// Perform some checks
#ifdef CHECKS
	if (int(r*rDelta_) < 0)
	{
		Messenger::print("BAD_VALUE - Bin value of r is negative (%i) in PairPotential::energy.\n", int(r*rDelta_));
		return 0.0;
	}
#endif

	// Return interpolated value
	return uFull_.interpolated(r, r*rDelta_);
}

// Return analytic potential at specified r
double PairPotential::analyticEnergy(double r)
{
	if (r > range_) return 0.0;

	// Short-range potential
	double energy = analyticShortRangeEnergy(r, shortRangeType_);

	// Coulomb contribution
	energy += analyticCoulombEnergy(chargeI_*chargeJ_, r);

	return energy;
}

// Return analytic coulomb potential energy of specified charges
double PairPotential::analyticCoulombEnergy(double qiqj, double r, PairPotential::CoulombTruncationScheme truncation)
{
	// Calculate based on truncation scheme
	if (truncation == PairPotential::NoCoulombTruncation) return COULCONVERT * qiqj / r;
	else if (truncation == PairPotential::ShiftedCoulombTruncation) return COULCONVERT * qiqj * (1.0/r + r/(range_*range_) - 2.0/range_);

	return 0.0;
}

// Return derivative at specified r
double PairPotential::force(double r)
{
	// Perform some checks
#ifdef CHECKS
	if (int(r*rDelta_) < 0)
	{
		Messenger::print("BAD_VALUE - Bin value of r is negative (%i) in PairPotential::force.\n", int(r*rDelta_));
		return 0.0;
	}
#endif

	// Return interpolated value
	return dUFull_.interpolated(r, r*rDelta_);
}

// Return analytic derivative of potential at specified r
double PairPotential::analyticForce(double r)
{
	if (r > range_) return 0.0;

	// Short-range potential
	double force = analyticShortRangeForce(r, shortRangeType_);

	// Coulomb contribution
	force += analyticCoulombForce(chargeI_*chargeJ_, r);

	return force;
}

// Return analytic coulomb force of specified charges
double PairPotential::analyticCoulombForce(double qiqj, double r, PairPotential::CoulombTruncationScheme truncation)
{
	// Calculate based on truncation scheme
	if (truncation == PairPotential::NoCoulombTruncation) return -COULCONVERT * qiqj / (r*r);
	else if (truncation == PairPotential::ShiftedCoulombTruncation) return -COULCONVERT * qiqj * (1.0/(r*r) - 1.0/(range_*range_));

	return 0.0;
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

// Return original potential
XYData& PairPotential::uOriginal()
{
	return uOriginal_;
}

// Return additional potential
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

// Set additional potential
void PairPotential::setUAdditional(XYData& newUAdditional)
{
	uAdditional_ = newUAdditional;

	calculateUFull();
	calculateDUFull();
}

// Adjust additional potential, and recalculate UFull and dUFull
void PairPotential::adjustUAdditional(XYData& u, double factor)
{
	uAdditional_.addInterpolated(u, factor);

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
	parser.writeLineF("#%9s  %12s  %12s  %12s  %12s  %12s  %12s  %12s  %12s\n", "r(Angs)", "U(kJ/mol)", "dU(kJ/mol/Ang)", "U(kJ/mol)", "U(kJ/mol)", "U(kJ/mol)", "dU(kJ/mol/Ang)", "rFine(Angs)", "Derivative");
	double fineDelta = delta_*0.05;
	for (int n = 0; n<nPoints_; ++n) parser.writeLineF("%10.6e  %12.6e  %12.6e  %12.6e  %12.6e  %12.6e  %12.6e\n", uOriginal_.x(n), uFull_.y(n), dUFull_.y(n), uOriginal_.y(n), uAdditional_.interpolated(uOriginal_.x(n)), analyticEnergy(uOriginal_.x(n)), analyticForce(uOriginal_.x(n)), fineDelta*n, dUFull_.x(fineDelta*n));

	parser.closeFiles();

	return true;
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool PairPotential::broadcast(ProcessPool& procPool, int root)
{
#ifdef PARALLEL
	// Need the master AtomType list ot proceed
	const List<AtomType>& masterAtomTypes = List<AtomType>::masterInstance();

	// PairPotential type
	if (!procPool.broadcast(EnumCast<PairPotential::ShortRangeType>(shortRangeType_), root)) return false;
	if (!procPool.broadcast(includeCoulomb_, root)) return false;

	// Source Parameters - Master needs to determine AtomType indices
	int index;
	if (procPool.isMaster()) index = masterAtomTypes.indexOf(atomTypeI_);
	if (!procPool.broadcast(index, root)) return false;
	atomTypeI_ = masterAtomTypes.item(index);
	if (procPool.isMaster()) index = masterAtomTypes.indexOf(atomTypeJ_);
	if (!procPool.broadcast(index, root)) return false;
	atomTypeJ_ = masterAtomTypes.item(index);
	if (!procPool.broadcast(parameters_, MAXSRPARAMETERS, root)) return false;
	if (!procPool.broadcast(chargeI_, root)) return false;
	if (!procPool.broadcast(chargeJ_, root)) return false;

	// Tabulation Parameters
	if (!procPool.broadcast(range_, root)) return false;
	if (!procPool.broadcast(nPoints_, root)) return false;
	if (!procPool.broadcast(delta_, root)) return false;
	if (!procPool.broadcast(rDelta_, root)) return false;

	// Tabulations
	uOriginal_.broadcast(procPool, root);
	uAdditional_.broadcast(procPool, root);
	uFull_.broadcast(procPool, root);
	dUFull_.broadcast(procPool, root);
#endif
	return true;
}
