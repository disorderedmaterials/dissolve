/*
	*** IsotopologueMix Definition
	*** src/classes/isotopologuemix.h
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

#ifndef DUQ_ISOTOPOLOGUEMIX_H
#define DUQ_ISOTOPOLOGUEMIX_H

#include "templates/list.h"
#include "templates/reflist.h"

// Forward Declarations
class Species;
class Isotopologue;

/*
 * IsotopologueMix Definition
 */
class IsotopologueMix : public ListItem<IsotopologueMix>
{
	public:
	// Constructor
	IsotopologueMix();
	// Destructor
	~IsotopologueMix();


	/*
	 * Isotopologue Mix
	 */
	private:
	// Associated Species
	Species* species_;
	// Isotopologue List
	RefList<Isotopologue,double> mix_;
	
	public:
	// Set associated Species
	void setSpecies(Species* sp);
	// Return associated Species
	Species* species() const;
	// Update Isotopologue RefList
	void update();
	// Add next available Isotopologue to list
	bool addNextIsotopologue();
	// Add specific Isotopologue to list
	bool addIsotopologue(Isotopologue* iso, double relPop);
	// Set Isotopologue component in list
	bool setIsotopologue(Isotopologue* iso, double relPop);
	// Remove Isotopologue component from list
	bool removeIsotopologue(Isotopologue* iso);
	// Return first Isotopologue component
	RefListItem<Isotopologue,double>* isotopologues() const;
	// Return nth Isotopologue component
	RefListItem<Isotopologue,double>* isotopologue(int n);
	// Return number of Isotopologues in mix
	int nIsotopologues() const;
	// Return whether the mix contains the specified Isotopologue
	RefListItem<Isotopologue,double>* hasIsotopologue(Isotopologue* iso) const;
	// Return total relative population
	double totalRelative() const;


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast();
};

#endif
