/*
	*** IsotopologueMix Definition
	*** src/classes/isotopologuemix.cpp
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

#include "classes/isotopologuemix.h"
#include "classes/species.h"
#include "base/comms.h"

// Constructor
IsotopologueMix::IsotopologueMix() : ListItem<IsotopologueMix>()
{
	species_ = NULL;
}

// Destructor
IsotopologueMix::~IsotopologueMix()
{
}

/*
 * Isotopologue Mix
 */

// Set associated Species
void IsotopologueMix::setSpecies(Species* sp)
{
	species_ = sp;
}

// Return associated Species
Species* IsotopologueMix::species() const
{
	return species_;
}

// Update Isotopologue RefList
void IsotopologueMix::update()
{
	// Go through list of Isotopologues present in this mix, removing any that no longer exist
	RefListItem<Isotopologue,double>* ri = mix_.first(), *next;
	while (ri)
	{
		next = ri->next;
		// Check parent Species for presence of this Isotopologue
		if (!species_->hasIsotopologue(ri->item))
		{
			mix_.remove(ri);
			msg.print("Removed Isotopologue from mixture for Species '%s' since it no longer existed.\n", species_->name());
		}
		ri = next;
	}
}

// Add next available Isotopologue to list
bool IsotopologueMix::addNextIsotopologue()
{
	// NULL Pointer?
	if (species_ == NULL)
	{
		msg.error("NULL_POINTER - NULL Species pointer in IsotopologueMix::addNextIsotopologue().\n");
		return false;
	}
	
	// Check to see if the are any Isotopologues available to add
	if (mix_.nItems() == species_->nIsotopologues())
	{
		msg.print("Warning: Can't add another Isotopologue to the mixture since there are none left for Species '%s'.\n", species_->name());
		return false;
	}
	
	// Find unique (unused) Isotopologue
	Isotopologue* iso;
	for (iso = species_->isotopologues(); iso != NULL; iso = iso->next) if (!mix_.contains(iso)) break;

	if (iso == NULL)
	{
		msg.error("Couldn't find an unused Isotopologue in Species '%s'.\n", species_->name());
		return false;
	}
	
	mix_.add(iso, 1.0);
	
	return true;
}

// Add specific Isotopologue to list
bool IsotopologueMix::addIsotopologue(Isotopologue* iso, double relPop)
{
	// Search current list to see if the specified Isotopologue already exists
	if (hasIsotopologue(iso))
	{
		msg.error("Can't add Isotopologue '%s' (of Species '%s') to Sample since it is already there.\n", iso->name(), species_->name());
		return false;
	}

	mix_.add(iso, relPop);
	return true;
}

// Set Isotopologue component in list
bool IsotopologueMix::setIsotopologue(Isotopologue* iso, double relPop)
{
	// NULL Pointer?
	if (iso == NULL)
	{
		msg.error("NULL_POINTER - NULL Isotopologue passed to IsotopologueMix::setIsotopologue().\n");
		return false;
	}
	
	// Find this Isotopologue in the list
	RefListItem<Isotopologue,double>* tope = mix_.contains(iso);
	if (tope == NULL)
	{
		msg.warn("Warning: IsotopologueMix doest not contain the Isotopologue '%s', so its fraction can't be set.\n", iso->name());
		return false;
	}
	tope->data = relPop;
	return true;
}

// Return first Isotopologue component
RefListItem<Isotopologue,double>* IsotopologueMix::isotopologues() const
{
	return mix_.first();
}

// Return nth Isotopologue component
RefListItem<Isotopologue,double>* IsotopologueMix::isotopologue(int n)
{
	return mix_[n];
}

// Return number of Isotopologues in list
int IsotopologueMix::nIsotopologues() const
{
	return mix_.nItems();
}

// Return whether the mix contains the specified Isotopologue
RefListItem<Isotopologue,double>* IsotopologueMix::hasIsotopologue(Isotopologue* iso) const
{
	return mix_.contains(iso);
}

// Return total relative population
double IsotopologueMix::totalRelative() const
{
	double total = 0.0;
	for (RefListItem<Isotopologue,double>* ri = mix_.first(); ri != NULL; ri = ri->next) total += ri->data;
	return total;
}
