/*
	*** AtomTypeIndex Definition
	*** src/lib/classes/atomtypeindex.h
	Copyright T. Youngs 2012-2014

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

#ifndef DUQ_ATOMTYPEINDEX_H
#define DUQ_ATOMTYPEINDEX_H

#include "templates/list.h"
#include "templates/array.h"

// Forward Declarations
class AtomType;
class Isotope;

/*!
 * \brief AtomTypeData Definition
 * \details AtomTypeData is a simple class which stores a pointer to a reference AtomType along with other related data such as
 * its (fractional) population within the system.
 */
class AtomTypeData : public ListItem<AtomTypeData>
{
	public:
	// Constructor
	AtomTypeData();


	/*!
	 * \name Properties
	 * \details The stored AtomType has associated with at least one Isotopes, depending on whether 
	 * a mixture of isotopically-substituted AtomTypes exists in the Sample. Individual populations of each Isotope
	 * are maintained, and fractional world populations are calculated by AtomTypeIndex::finalise() once the list is 
	 * complete.
	 */
	///@{
	private:
	// Reference AtomType
	AtomType* atomType_;
	// Isotopes
	Isotope* isotope_;
	// Integer populations of isotopes
	int population_;
	// Fractional populations of isotopes
	double fraction_;
	// Index of non-isotopic master type
	int masterIndex_;

	public:
	// Initialise
	bool initialise(AtomType* atomType, Isotope* topeA);
	// Add to population of Isotope
	void add(int nAdd);
	// Return reference AtomType
	AtomType* atomType() const;
	// Finalise, calculating fractional populations etc.
	void finalise(int totalAtoms);
	// Return Isotope
	Isotope* isotope() const;
	// Return population
	int population() const;
	// Return fractional population
	double fraction() const;
	// Return referenced AtomType name
	const char* name();
	// Set index of non-isotopic master type
	void setMasterIndex(int id);
	// Return index of non-isotopic master type
	int masterIndex();
	///@}
};

/*!
 * \brief AtomTypeIndex Definition
 * \details AtomTypeIndex stores and maintains a unique list of AtomType/Isotope combinations, and is used, for example, to
 * generate an array of partial RDF / S(Q) data in a Model or SampleData.
 */
class AtomTypeIndex
{
	public:
	// Constructor
	AtomTypeIndex();
	// Destructor
	~AtomTypeIndex();


	/*!
	 * \name Type List
	 */
	///@{
	private:
	// List of AtomTypeData
	List<AtomTypeData> types_;

	public:
	// Clear all data
	void clear();
	// Add/increase this AtomType/Isotope pair
	void add(AtomType* atomType, Isotope* tope, int popAdd = 0);
	// Return number of AtomType/Isotopes in list
	int nItems() const;
	// Return first item in list
	AtomTypeData* first() const;
	// Print AtomType populations
	void print();
	///@}


	/*!
	 * \name Access
	 */
	///@{
	public:
	// Return index of AtomType/Isotope in list
	int indexOf(AtomType* atomtype) const;
	// Return total population of all types in list
	int totalPopulation() const;
	// Finalise list, calculating fractional populations etc.
	void finalise();
	// Return nth referenced AtomType
	AtomType* atomType(int n);
	// Array access operator
	AtomTypeData* operator[](int n);
	///@}
};

#endif
