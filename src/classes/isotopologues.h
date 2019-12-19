/*
	*** Isotopologues Definition
	*** src/classes/isotopologues.h
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

#ifndef DISSOLVE_ISOTOPOLOGUES_H
#define DISSOLVE_ISOTOPOLOGUES_H

#include "genericitems/base.h"
#include "templates/listitem.h"
#include "templates/refdatalist.h"

// Forward Declarations
class Species;
class Isotopologue;
class ProcessPool;
class LineParser;

// Isotopologues
class Isotopologues : public ListItem<Isotopologues>, public GenericItemBase
{
	public:
	// Constructor
	Isotopologues();
	// Destructor
	~Isotopologues();


	/*
	 * Isotopologue Mix
	 */
	private:
	// Associated Species
	Species* species_;
	// Integer population of associated Species
	int speciesPopulation_;
	// Isotopologue List
	RefDataList<const Isotopologue,double> mix_;
	
	public:
	// Set associated Species and population
	void setSpecies(Species* sp, int population);
	// Return associated Species
	Species* species() const;
	// Return associated Species population
	int speciesPopulation() const;
	// Update Isotopologue RefList
	void update();
	// Add next available Isotopologue to list
	bool addNextIsotopologue();
	// Add specific Isotopologue to list
	bool addIsotopologue(const Isotopologue* iso, double relPop);
	// Set Isotopologue component in list
	bool setIsotopologue(const Isotopologue* iso, double relPop);
	// Remove Isotopologue component from list
	bool removeIsotopologue(const Isotopologue* iso);
	// Return Isotopologue components
	const RefDataList<const Isotopologue,double>& isotopologues() const;
	// Return nth Isotopologue component
	RefDataItem<const Isotopologue,double>* isotopologue(int n);
	// Return number of Isotopologues in mix
	int nIsotopologues() const;
	// Return whether the mix contains the specified Isotopologue
	RefDataItem<const Isotopologue,double>* hasIsotopologue(const Isotopologue* iso) const;
	// Return total relative population
	double totalRelative() const;
	// Normalise total relative population to 1.0
	void normalise();


	/*
	 * GenericItemBase Implementations
	 */
	public:
	// Return class name
	static const char* itemClassName();
	// Read data through specified LineParser
	bool read(LineParser& parser, const CoreData& coreData);
	// Write data through specified LineParser
	bool write(LineParser& parser);


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data
	bool broadcast(ProcessPool& procPool, const int root, const CoreData& coreData);
	// Check item equality
	bool equality(ProcessPool& procPool);
};

#endif
