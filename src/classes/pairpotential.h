/*
	*** Interatomic Pair Potential
	*** src/classes/pairpotential.h
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

#ifndef DUQ_PAIRPOTENTIAL_H
#define DUQ_PAIRPOTENTIAL_H

#include "base/xydata.h"
#include "base/parameters.h"
#include "templates/list.h"

// Forward Declarations
class AtomType;

// PairPotential Definition
class PairPotential : public ListItem<PairPotential>
{
	public:
	// Constructor
	PairPotential();
	// ShortRange Interaction Type enum
	enum ShortRangeType
	{
		NoInteractionType,		/* No short-range dispersive forces */
		LennardJonesType,		/* Lennard-Jones 12-6 form with Lorentz-Berthelot combination rules */
		LennardJonesGeometricType,	/* Lennard-Jones 12-6 form with Geometric combination rules */
		nShortRangeTypes		/* Number of short-range interaction types */
	};
	// Convert text string to ShortRangeType
	static ShortRangeType shortRangeType(const char* s);
	// Convert ShortRangeType to text string
	static const char* shortRangeType(ShortRangeType id);
	// Truncation Scheme enum
	enum TruncationScheme
	{
		NoTruncation,			/* No truncation scheme */
		ShiftedTruncation,		/* Shifted and truncated */
		nTruncationSchemes		/* Number of truncation schemes */
	};
	// Convert text string to TruncationScheme
	static TruncationScheme truncationScheme(const char* s);
	// Convert TruncationScheme to text string
	static const char* truncationScheme(TruncationScheme id);


	/*
	 * Seed Interaction Type
	 */
	private:
	// Short range type
	ShortRangeType shortRangeType_;
	// Whether Coulomb term should be included in the generated potential
	bool includeCoulomb_;
	// Truncation scheme to apply to generated Coulomb potential
	static TruncationScheme coulombTruncationScheme_;
	
	public:
	// Set short-ranged type
	void setShortRangeType(ShortRangeType type);
	// Return short-ranged type
	ShortRangeType shortRangeType() const;
	// Set whether Coulomb term should be included in the generated potential
	void setIncludeCoulomb(bool b);
	// Return whether Coulomb term should be included in the generated potential
	bool includeCoulomb();
	// Set Coulomb truncation scheme
	static void setCoulombTruncationScheme(TruncationScheme scheme);
	// Return Coulomb truncation scheme
	static TruncationScheme coulombTruncationScheme();


	/*
	 * Source Parameters
	 */
	private:
	// Original source AtomTypes
	AtomType* atomTypeI_, *atomTypeJ_;
	// Parameters (determined from source AtomTypes
	double parameters_[MAXSRPARAMETERS];
	// Charge on I (taken from AtomType)
	double chargeI_;
	// Charge on J (taken from AtomType)
	double chargeJ_;
	
	private:
	// Set XYData names from source AtomTypes
	void setXYDataNames();
	
	public:
	// Set source parameters from AtomTypes
	bool setParameters(AtomType* typeI, AtomType* typeJ);
	// Set source AtomType pointers only
	void setAtomTypes(AtomType* typeI, AtomType* typeJ);
	// Return first AtomType name
	const char* atomTypeNameI() const;
	// Return second AtomType name
	const char* atomTypeNameJ() const;
	// Return first source AtomType
	AtomType* atomTypeI() const;
	// Return second source AtomType
	AtomType* atomTypeJ() const;
	// Set parameter specified
	void setParameter(int index, double value);
	// Return parameter specified
	double parameter(int index);
	// Set charge I
	void setChargeI(double value);
	// Return charge I
	double chargeI() const;
	// Set charge J
	void setChargeJ(double value);
	// Return charge J
	double chargeJ() const;


	/*
	 * Tabulated Potential
	 */
	private:
	// Number of points to tabulate
	int nPoints_;
	// Maximum distance of potential
	double range_;
	// Distance between points in tabulated potentials
	double delta_, rDelta_;
	// Truncation width for LJ part of potential
	double truncationWidth_;
	// Tabulated original potential, calculated from AtomType parameters
	XYData uOriginal_;
	// Additional potential, generated by some means
	XYData uAdditional_;
	// Full potential (original plus additional), used in simulations
	XYData uFull_;
	// Tabulated derivative of full potential
	XYData dUFull_;
	
	private:
	// Return analytic short range potential energy
	double analyticShortRangeEnergy(double r, PairPotential::ShortRangeType type);
	// Return analytic short range force
	double analyticShortRangeForce(double r, PairPotential::ShortRangeType type);
	// Calculate full potential
	void calculateUFull();
	// Calculate derivative of potential
	void calculateDUFull();

	public:
	// Set up and perform initial generation of potential
	bool setUp(double maxR, double truncationWidth, double delta, bool includeCoulomb);
	// Return range of potential
	double range() const;
	// (Re)generate original potential (uOriginal) from current parameters
	bool calculateUOriginal(bool recalculateUFull = true);
	// Return potential at specified r
	double energy(double r);
	// Return analytic potential at specified r
	double analyticEnergy(double r);
	// Return analytic coulomb potential energy of specified charge product
	double analyticCoulombEnergy(double qiqj, double r);
	// Return derivative of potential at specified r
	double force(double r);
	// Return analytic derivative of potential at specified r
	double analyticForce(double r);
	// Return analytic coulomb force of specified charge product
	double analyticCoulombForce(double qiqj, double r);
	// Return full tabulated potential (original plus additional)
	XYData& uFull();
	// Return full tabulated derivative
	XYData& dUFull();
	// Return original potential
	XYData& uOriginal();
	// Return additional potential
	XYData& uAdditional();
	// Zero additional potential
	void resetUAdditional();
	// Set additional potential
	void setUAdditional(XYData& newUAdditional);
	// Adjust additional potential, and recalculate UFull and dUFull
	void adjustUAdditional(XYData& u, double factor = 1.0);


	/*
	 * File I/O
	 */
	public:
	// Save PairPotential data to specified file
	bool save(const char* filename);


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(ProcessPool& procPool, int root = 0);
};

#endif
