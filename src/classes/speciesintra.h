/*
	*** SpeciesIntra Definition
	*** src/classes/speciesintra.h
	Copyright T. Youngs 2012-2019

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

#ifndef DISSOLVE_SPECIESINTRA_H
#define DISSOLVE_SPECIESINTRA_H

#include "base/charstring.h"
#include "templates/array.h"
#include "templates/reflist.h"

#define MAXINTRAPARAMS 4

// Forward Declarations
class SpeciesAtom;
class Species;
class MasterIntra;

/*
 * SpeciesIntra Definition
 */
class SpeciesIntra
{
	public:
	// Constructor
	SpeciesIntra();
	// Destructor
	virtual ~SpeciesIntra();
	// Interaction Type
	enum IntramolecularType
	{
		IntramolecularBond,		/* Interaction is a bond between two atoms in the same molecule */
		IntramolecularAngle,		/* Interaction is an angle between three atoms in the same molecule */
		IntramolecularTorsion,		/* Interaction is a torsion between four atoms in the same molecule */
		IntramolecularImproper		/* Interaction is an improper torsion between four atoms in the same molecule */
	};


	/*
	 * Basic Data
	 */
	protected:
	// Parent Species
	Species* parent_;

	public:
	// Set parent Species
	void setParent(Species* parent);
	// Return parent Species
	Species* parent() const;


	/*
	 * Interaction Parameters
	 */
	protected:
	// Linked master from which parameters should be taken (if relevant)
	MasterIntra* masterParameters_;
	// Index of functional form of interaction
	int form_;
	// Parameters for interaction
	double parameters_[MAXINTRAPARAMS];

	public:
	// Set linked master from which parameters should be taken
	void setMasterParameters(MasterIntra* master);
	// Return linked master from which parameters should be taken
	MasterIntra* masterParameters();
	// Detach from MasterIntra, if we are currently referencing one
	void detachFromMasterIntra();
	// Return parameter source
	SpeciesIntra* parameterSource();
	// Set functional form index of interaction
	void setForm(int form);
	// Return functional form index of interaction
	int form() const;
	// Set nth parameter
	void setParameter(int id, double value);
	// Set all parameters
	void setParameters(double a, double b = 0.0, double c = 0.0, double d = 0.0);
	// Return nth parameter
	double parameter(int id) const;
	// Return array of parameters
	const double* parameters() const;
	// Return parameters as Array<double>
	Array<double> parametersAsArray() const;
	// Set parameters from double*
	void setParameters(Array<double> params);
	// Set parameters from Array<double>
	void setParameters(const double* params);
	// Set up any necessary parameters
	virtual void setUp() = 0;
	// Calculate and return fundamental frequency for the interaction
	virtual double fundamentalFrequency(double reducedMass) const = 0;
	// Return type of this interaction
	virtual IntramolecularType type() const = 0;


	/*
	 * Connections
	 */
	private:
	// Number of SpeciesAtoms attached to termini (number of items stored in attached_ arrays)
	int nAttached_[2];
	// Arrays of indices (in)directly attached to termini
	int* attached_[2];
	// Size of attached_ SpeciesAtoms arrays (maximum number of items that may be stored)
	int arraySize_[2];
	// Whether the term is contained within a cycle
	bool inCycle_;

	public:
	// Clear and delete all arrays
	void deleteAttachedAtomArrays();
	// Set attached SpeciesAtoms for terminus specified
	void setAttachedAtoms(int terminus, const RefList<SpeciesAtom>& atoms);
	// Set attached SpeciesAtoms for terminus specified (single SpeciesAtom)
	void setAttachedAtoms(int terminus, SpeciesAtom* atom);
	// Return number of attached SpeciesAtoms for terminus specified
	int nAttached(int terminus) const;
	// Return array of attached indices for terminus specified
	int* attached(int terminus) const;
	// Set whether the term is contained within a cycle
	void setInCycle(bool b);
	// Return whether the term is contained within a cycle
	bool inCycle() const;
};

#endif
