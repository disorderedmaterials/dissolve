/*
	*** Universal Forcefield
	*** src/data/uff.h
	Copyright T. Youngs 2019

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

#ifndef DISSOLVE_UFF_H
#define DISSOLVE_UFF_H

#include "data/ff.h"
#include "templates/array.h"
#include "templates/list.h"

// Forward Declarations
class CoreData;
class Species;
class SpeciesAtom;

// Universal Forcefield AtomType Data
class UFFAtomType : public ElementReference, public ListItem<UFFAtomType>
{
	public:
	// Constructor
	UFFAtomType(int z = 0, const char* symbol = NULL, int index = -1, const char* name = NULL, double r = 0.0, double theta = 0.0, double x = 0.0, double D = 0.0, double zeta = 0.0, double Z = 0.0, double chi = 0.0, int geom = 0, double V = 0.0, double U = 0.0);
	// Assignment Operator
	UFFAtomType& operator=(const UFFAtomType& source);


	/*
	 * Data
	 */
	private:
	// Index of atom type
	int index_;
	// Name of atom type
	const char* name_;
	// Generator parameters
	double r_, theta_, x_, D_, zeta_, Z_, chi_, V_, U_;
	int geom_;

	public:
	// Return index of atom type
	int index() const;
	// Return name of atom type
	const char* name() const;
	// Return number of bonds that the atom type should have
	int nBonds() const;
};

// Universal Forcefield
class UFF : public Forcefield
{
	/*
	 * Definition
	 */
	public:
	// Return name of Forcefield
	const char* name();


	/*
	 * Atom Type Data
	 */
	private:
	// Atom type data, grouped by element
	static Array< List<UFFAtomType> > atomTypesByElementPrivate_;

	private:
	// Return atom type data for specified Element
	static List<UFFAtomType>& atomTypesByElement(int Z);

	public:
	// Register specified atom type to given Element
	static void registerAtomType(UFFAtomType* atomType, int Z);
	// Return the named UFFAtomType (if it exists)
	UFFAtomType* atomTypeByName(const char* name, Element* element = NULL);


	/*
	 * Term Generation
	 */
	public:
	// Determine and return AtomType for specified SpeciesAtom
	UFFAtomType* determineAtomType(SpeciesAtom* sp);
	// Create and assign suitable AtomTypes for the supplied Species
	bool createAtomTypes(Species* sp, CoreData& coreData);
	// Generate missing bond terms for the supplied Species
	bool generateBondTerms(Species* sp);
	// Generate missing angle terms for the supplied Species
	bool generateAngleTerms(Species* sp);
	// Generate missing torsion terms for the supplied Species
	bool generateTorsionTerms(Species* sp);
};

#endif
