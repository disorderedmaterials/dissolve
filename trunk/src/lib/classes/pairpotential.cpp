/*
	*** Pair Potential Definition
	*** src/lib/classes/pairpotential.cpp
	Copyright T. Youngs 2012-2013

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
#include "base/comms.h"
#include "base/flag.h"
#include <math.h>
#include <string.h>

/*!
 * \brief Constructor
 * \details Constructor for PairPotential. 
 */
PairPotential::PairPotential() : ListItem<PairPotential>()
{
	sigmaIJ_ = 0.0;
	epsilonIJ_ = 0.0;
	chargeI_ = 0.0;
	chargeJ_ = 0.0;
	nPoints_ = 0;
	delta_ = -1.0;
	type_ = DispersionType;
}

// Input File Block Keywords
const char *PairPotentialTypeKeywords[] = { "Coulomb", "Dispersion", "Full" };

/*!
 * \brief Convert text string to PairPotentialType
 */
PairPotential::PairPotentialType PairPotential::pairPotentialType(const char* s)
{
	for (int n=0; n<PairPotential::nPairPotentialTypes; ++n) if (strcmp(s,PairPotentialTypeKeywords[n]) == 0) return (PairPotential::PairPotentialType) n;
	return PairPotential::nPairPotentialTypes;
}

/*!
 * \brief Convert PairPotentialType to text string
 */
const char *PairPotential::pairPotentialType(PairPotential::PairPotentialType id)
{
	return PairPotentialTypeKeywords[id];
}

/*
// Potential Type
*/

/*!
 * \brief Set PairPotential type
 */
void PairPotential::setType(PairPotential::PairPotentialType type)
{
	SET_MODIFIED
	type_ = type;
}

/*!
 * \brief Return PairPotential type
 */
PairPotential::PairPotentialType PairPotential::type() const
{
	return type_;
}

/*
// Source Parameters
*/

/*!
 * \brief Set Data2D names from source AtomTypes
 */
void PairPotential::setData2DNames()
{
	// Check for NULL pointers
	if (atomTypeI_ == NULL)
	{
		msg.error("NULL_POINTER - NULL AtomType pointer (atomTypeI_) found in PairPotential::setData2DNames().\n");
		return;
	}
	if (atomTypeJ_ == NULL)
	{
		msg.error("NULL_POINTER - NULL AtomType pointer (atomTypeJ_) found in PairPotential::setData2DNames().\n");
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

/*!
 * \brief Set source parameters from AtomTypes
 */
void PairPotential::setParameters(AtomType* typeI, AtomType* typeJ)
{
	// Check for NULL pointers
	if (typeI == NULL)
	{
		msg.error("NULL_POINTER - NULL AtomType pointer (typeI) given to PairPotential::setParameters().\n");
		return;
	}
	if (typeJ == NULL)
	{
		msg.error("NULL_POINTER - NULL AtomType pointer (typeJ) given to PairPotential::setParameters().\n");
		return;
	}
	
	atomTypeI_ = typeI;
	atomTypeJ_ = typeJ;
	setData2DNames();
	Parameters* paramsI = atomTypeI_->parameters();
	Parameters* paramsJ = atomTypeJ_->parameters();
	if ((paramsI == NULL) || (paramsJ == NULL))
	{
		msg.error("NULL_POINTER - One or both AtomTypes ('%s' and '%s') given to PairPotential::setParameters() contain a NULL Parameters pointer.\n", atomTypeI_->name(), atomTypeJ_->name());
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

/*!
 * \brief Set source AtomType pointers only
 */
void PairPotential::setAtomTypes(AtomType* typeI, AtomType* typeJ)
{
	// Check for NULL pointers
	if (typeI == NULL)
	{
		msg.error("NULL_POINTER - NULL AtomType pointer (typeI) given to PairPotential::setAtomTypes().\n");
		return;
	}
	if (typeJ == NULL)
	{
		msg.error("NULL_POINTER - NULL AtomType pointer (typeJ) given to PairPotential::setAtomTypes().\n");
		return;
	}

	atomTypeI_ = typeI;
	atomTypeJ_ = typeJ;
	setData2DNames();
}

/*!
 * \brief Return first AtomType name
 */
const char* PairPotential::atomTypeNameI() const
{
	// Check for NULL pointers
	if (atomTypeI_ == NULL)
	{
		msg.error("NULL_POINTER - NULL AtomType pointer found in PairPotential::atomTypeNameI().\n");
		return "NULL";
	}
	return atomTypeI_->name();
}

/*!
 * \brief Return second AtomType name
 */
const char* PairPotential::atomTypeNameJ() const
{
	// Check for NULL pointers
	if (atomTypeJ_ == NULL)
	{
		msg.error("NULL_POINTER - NULL AtomType pointer found in PairPotential::atomTypeNameJ().\n");
		return "NULL";
	}
	return atomTypeJ_->name();
}

/*!
 * \brief Return first source AtomType
 */
AtomType* PairPotential::atomTypeI() const
{
	return atomTypeI_;
}

/*!
 * \brief Return second source AtomType
 */
AtomType* PairPotential::atomTypeJ() const
{
	return atomTypeJ_;
}

/*!
 * \brief Set sigma
 */
void PairPotential::setSigmaIJ(double value)
{
	SET_MODIFIED
	sigmaIJ_ = value;
}

/*!
 * \brief Return sigma
 */
double PairPotential::sigmaIJ() const
{
	return sigmaIJ_;
}

/*!
 * \brief Set epsilon
 */
void PairPotential::setEpsilonIJ(double value)
{
	SET_MODIFIED
	epsilonIJ_ = value;
}

/*!
 * \brief Return epsilon
 */
double PairPotential::epsilonIJ() const
{
	return epsilonIJ_;
}

/*!
 * \brief Set charge I
 */
void PairPotential::setChargeI(double value)
{
	SET_MODIFIED
	chargeI_ = value;
}

/*!
 * \brief Return charge I
 */
double PairPotential::chargeI() const
{
	return chargeI_;
}

/*!
 * \brief Set charge J
 */
void PairPotential::setChargeJ(double value)
{
	SET_MODIFIED
	chargeJ_ = value;
}

/*!
 * \brief Return charge J
 */
double PairPotential::chargeJ() const
{
	return chargeJ_;
}

/*
// Tabulated PairPotential
*/

/*!
 * \brief Regenerate derivative data
 */
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

/*!
 * \name Generate potential
 */
bool PairPotential::generate(double maxR, double truncationWidth, double delta)
{
	// Check that AtomType pointers were set at some pointer
	if ((atomTypeI_ == NULL) || (atomTypeJ_ == NULL))
	{
		msg.error("NULL_POINTER - One or both AtomTypes in PairPotential are NULL.\n");
		return FALSE;
	}

	// Determine nPoints_
	delta_ = delta;
	rDelta_ = 1.0/delta_;
	nPoints_ = (maxR*maxR) / delta_;
	
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

	// Generate derivative data
	calculateDerivative();

	return TRUE;
}

/*!
 * \brief Return potential at specified r-squared
 */
double PairPotential::energyAtRSquared(double distanceSq) const
{
	// Perform some checks
#ifdef CHECKS
	if (int(distanceSq*rDelta_) < 0)
	{
		msg.print("BAD_VALUE - Bin value of rSq is negative (%i) in PairPotential::energyAtRSquared.\n", int(distanceSq*rDelta_));
		return 0.0;
	}
#endif
	// Determine potential bin
	int bin = distanceSq*rDelta_;
	return (bin >= nPoints_ ? 0.0 : u_.y(bin));
}

/*!
 * \brief Return derivative at specified r-squared
 */
double PairPotential::forceAtRSquared(double distanceSq) const
{
	// Perform some checks
#ifdef CHECKS
	if (int(distanceSq*rDelta_) < 0)
	{
		msg.print("BAD_VALUE - Bin value of rSq is negative (%i) in PairPotential::forceAtRSquared.\n", int(distanceSq*rDelta_));
		return 0.0;
	}
#endif
	// Determine potential bin
	int bin = distanceSq*rDelta_;
	return (bin >= nPoints_ ? 0.0 : dU_.y(bin));
}

/*!
 * \brief Update perturbation to potential, recreating tabulated data
 */
bool PairPotential:: updatePerturbation(Data2D& perturbation, double maxAddition, double maxTotal)
{
	// Add this perturbation to the stack
// 	Data2D* p = perturbationStack_.add();
// 	(*p) = perturbation;

	// Rescale magnitude of perturbation
// 	p->arrayY() *= weight;

	// Do we have five items yet?
// 	if (perturbationStack_.nItems() < 2) return TRUE;

	// Create a new perturbation based on the data in the list
// 	Data2D newPerturbation = perturbation;
// 	newPerturbation.arrayY() = 0.0;
// 	for (Data2D* dat = perturbationStack_.first(); dat != NULL; dat = dat->next) newPerturbation.arrayY() += dat->arrayY();
// 	newPerturbation.arrayY() /= perturbationStack_.nItems();
// 	newPerturbation.smooth(3);
// 	newPerturbation.interpolate();
// 	perturbationStack_.clear();
// 	v_ += newPerturbation;
// 	v_.interpolate();

	// Determine 'magnitude' of current perturbation and new addition
	double currentMag = v_.absIntegral();
	Data2D addV = perturbation;
	double addMag = addV.absIntegral();

	// Scale new perturbation if it is greated than the allowed limit to add in one step
	msg.print("AddMAG = %f\n", addMag);
	if (addMag > maxAddition)
	{
		addV.arrayY() *= maxAddition / addMag;
		addMag = addV.absIntegral();
		printf("Too big!  New mag = %f\n", addMag);
	}
	
	// Check current v_ magnitude - if it is greater than the maximum allowed, rescale it...
	msg.print("currentMag = %f\n", currentMag);
	if ((currentMag + addMag) > maxTotal)
	{
		v_.arrayY() *= maxTotal / currentMag;
		currentMag = v_.absIntegral();
		msg.print("Rescaled value of v_ = %f\n", currentMag);
	}

	// Add new perturbation 
	v_ += addV;

	// Modify pair potential
	u_ = originalU_;
// 	for (int n=0; n<u_.nPoints(); ++n) u_.arrayY()[n] += v_.interpolated(sqrt(u_.x(n)));
	for (int n=0; n<u_.nPoints(); ++n) u_.arrayY()[n] *= 1.0 + v_.interpolated(sqrt(u_.x(n)));
	calculateDerivative();

	return TRUE;
}

/*!
 * \brief Clear perturbation to potential, reverting to original
 */
void PairPotential::clearPerturbation()
{
	u_ = originalU_;
	calculateDerivative();
}

/*!
 * \brief Return tabulated potential
 */
Data2D& PairPotential::u()
{
	return u_;
}

/*!
 * \brief Return tabulated derivative
 */
Data2D& PairPotential::dU()
{
	return dU_;
}

/*!
 * \brief Return original potential (without modifications)
 */
Data2D& PairPotential::originalU()
{
	return originalU_;
}

/*!
 * \brief Return modification to PairPotential
 */
Data2D& PairPotential::v()
{
	return v_;
}

/*
// File I/O
*/

// Save PairPotential data to specified file
bool PairPotential::save(const char* fileName)
{
	// Only the Master process can do this
	if (!Comm.master()) return TRUE;

	// Open file and check that we're OK to proceed writing to it
	LineParser parser;
	msg.print("Writing PairPotential file '%s'...\n", fileName);

	parser.openOutput(fileName, TRUE);
	if (!parser.isFileGoodForWriting())
	{
		msg.error("Couldn't open file '%s' for writing.\n", fileName);
		return FALSE;
	}
	
	parser.writeLineF("#%9s  %12s  %12s  %12s  %12s\n", "r(Angs)", "U(kJ/mol)", "dU(kJ/mol/Ang)", "U0(kJ/mol)", "V(kJ/mol)");
	if (v_.nPoints() > 0) for (int n = 0; n<nPoints_; ++n) parser.writeLineF("%10.4e  %12.4e  %12.4e  %12.4e  %12.4e\n", sqrt(u_.x(n)), u_.y(n), dU_.y(n), originalU_.y(n), v_.interpolated(sqrt(u_.x(n))));
	else for (int n = 0; n<nPoints_; ++n) parser.writeLineF("%10.4e  %12.4e  %12.4e  %12.4e  %12.4e\n", sqrt(u_.x(n)), u_.y(n), dU_.y(n), originalU_.y(n), 0.0);
	parser.closeFiles();
	return TRUE;
}

/*
// Parallel Comms
*/

/*!
 * \brief Broadcast data from Master to all Slaves
 */
bool PairPotential::broadcast(const List<AtomType>& atomTypes)
{
#ifdef PARALLEL
	// PairPotential type
	int n = type_;
	Comm.broadcast(&n, 1);
	type_ = (PairPotential::PairPotentialType) n;

	// Source Parameters - Master needs to determine AtomType indices
	int index;
	if (Comm.master()) index = atomTypes.indexOf(atomTypeI_);
	if (!Comm.broadcast(&index, 1)) return FALSE;
	atomTypeI_ = atomTypes.item(index);
	if (Comm.master()) index = atomTypes.indexOf(atomTypeJ_);
	if (!Comm.broadcast(&index, 1)) return FALSE;
	atomTypeJ_ = atomTypes.item(index);
	if (!Comm.broadcast(&sigmaIJ_, 1)) return FALSE;
	if (!Comm.broadcast(&epsilonIJ_, 1)) return FALSE;
	if (!Comm.broadcast(&chargeI_, 1)) return FALSE;
	if (!Comm.broadcast(&chargeJ_, 1)) return FALSE;

	// Tabulated Potential
	if (!Comm.broadcast(&nPoints_, 1)) return FALSE;
	if (!Comm.broadcast(&delta_, 1)) return FALSE;
	if (!Comm.broadcast(&rDelta_, 1)) return FALSE;
	u_.broadcast();
	originalU_.broadcast();
	dU_.broadcast();
#endif
	return TRUE;
}
