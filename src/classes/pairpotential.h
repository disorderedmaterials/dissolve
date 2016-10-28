/*
	*** Pair Potential Definition
	*** src/classes/pairpotential.h
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

#ifndef DUQ_PAIRPOTENTIAL_H
#define DUQ_PAIRPOTENTIAL_H

#include "base/data2d.h"
#include "templates/list.h"

// Forward Declarations
class AtomType;

/*
 * \brief PairPotential Definition
 * \details TODO
 */
class PairPotential : public ListItem<PairPotential>
{
	public:
	// Constructor
	PairPotential();
	// PairPotentialType enum
	enum PairPotentialType
	{
		CoulombType,		/* PairPotential which has only Coulomb interactions */
		DispersionType,		/* PairPotential which has only dispersive Lennard-Jones interactions */
		FullType,		/* PairPotential including both dispersive Lennard-Jones and Coulomb interactions */
		nPairPotentialTypes	/* Number of PairPotential types */
	};
	// Convert text string to PairPotentialType
	static PairPotentialType pairPotentialType(const char* s);
	// Convert PairPotentialType to text string
	static const char* pairPotentialType(PairPotentialType id);


	/*
	 * Potential Type
	 */
	private:
	// PairPotential type
	PairPotentialType type_;
	
	public:
	// Set PairPotential type
	void setType(PairPotentialType type);
	// Return PairPotential type
	PairPotentialType type() const;


	/*
	 * Source Parameters
	 */
	private:
	// Original source AtomTypes
	AtomType* atomTypeI_, *atomTypeJ_;
	// Sigma value (determined from AtomTypes)
	double sigmaIJ_;
	// Epsilon value (determined from AtomTypes)
	double epsilonIJ_;
	// Charge on I (taken from AtomType)
	double chargeI_;
	// Charge on J (taken from AtomType)
	double chargeJ_;
	
	private:
	// Set Data2D names from source AtomTypes
	void setData2DNames();
	
	public:
	// Set source parameters from AtomTypes
	void setParameters(AtomType* typeI, AtomType* typeJ);
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
	// Set sigma
	void setSigmaIJ(double value);
	// Return sigma
	double sigmaIJ() const;
	// Set epsilon
	void setEpsilonIJ(double value);
	// Return epsilon
	double epsilonIJ() const;
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
	double rangeSquared_;
	// Distance between points in tabulation and reciprocal
	double delta_, rDelta_;
	// Tabulated original potential, calculated from AtomType parameters
	Data2D originalU_;
	// Tabulated potential, used in simulations
	Data2D u_;
	// Tabulated derivative
	Data2D dU_;
	
	private:
	// Regenerate derivative data
	void calculateDerivative();

	public:
	// Generate potential
	bool generate(double maxR, double truncationWidth, double delta);
	// Return potential at specified r-squared
	double energyAtRSquared(double rSq) const;
	// Return derivative of potential at specified r-squared
	double forceAtRSquared(double rSq) const;
	// Return tabulated potential (inc. modifications)
	Data2D& u();
	// Return tabulated derivative (inc. modifications)
	Data2D& dU();
	// Return original potential (without modifications)
	Data2D& originalU();


	/*
	 * Perturbation
	 */
	private:
	// Additional potential
	Data2D v_;
	// List (stack) of perturbations to potential
	List<Data2D> perturbationStack_;
	// Range for LJ potential fit
	double ljFitMinimum_, ljFitMaximum_;

	public:
	// Update perturbation to potential, recreating tabulated data
	bool updatePerturbation(Data2D& perturbation, double yScale, double blendFactor);
	// Clear perturbation to potential, reverting to original
	void clearPerturbation();
	// Cost function callback (passed to Simplex on construction)
	double potentialFitCost(Array<double>& alpha);
	// Return modification to PairPotential
	Data2D& v();


	/*
	 * File I/O
	 */
	public:
	// Save PairPotential data to specified file
	bool save(const char* fileName);


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(ProcessPool& procPool, const List<AtomType>& atomTypes);
};

#endif
