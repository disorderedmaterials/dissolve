/*
	*** IsotopologueMix Definition
	*** src/classes/isotopologuemix.cpp
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

#include "classes/isotopologuemix.h"
#include "classes/species.h"
#include "base/processpool.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
IsotopologueMix::IsotopologueMix() : MPIListItem<IsotopologueMix>()
{
	species_ = NULL;
	speciesPopulation_ = 0;
}

// Destructor
IsotopologueMix::~IsotopologueMix()
{
}

/*
 * Isotopologue Mix
 */

// Set associated Species
void IsotopologueMix::setSpecies(Species* sp, int population)
{
	species_ = sp;
	speciesPopulation_ = population;
}

// Return associated Species
Species* IsotopologueMix::species() const
{
	return species_;
}

// Return associated Species population
int IsotopologueMix::speciesPopulation()
{
	return speciesPopulation_;
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
			Messenger::print("Removed Isotopologue from mixture for Species '%s' since it no longer existed.\n", species_->name());
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
		Messenger::error("NULL_POINTER - NULL Species pointer in IsotopologueMix::addNextIsotopologue().\n");
		return false;
	}
	
	// Check to see if the are any Isotopologues available to add
	if (mix_.nItems() == species_->nIsotopologues())
	{
		Messenger::warn("Can't add another Isotopologue to the mixture since there are none left for Species '%s'.\n", species_->name());
		return false;
	}
	
	// Find unique (unused) Isotopologue
	Isotopologue* iso;
	for (iso = species_->isotopologues(); iso != NULL; iso = iso->next) if (!mix_.contains(iso)) break;

	if (iso == NULL)
	{
		Messenger::error("Couldn't find an unused Isotopologue in Species '%s'.\n", species_->name());
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
		Messenger::error("Can't add Isotopologue '%s' (of Species '%s') to IsotopologueMix since it is already there.\n", iso->name(), species_->name());
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
		Messenger::error("NULL_POINTER - NULL Isotopologue passed to IsotopologueMix::setIsotopologue().\n");
		return false;
	}
	
	// Find this Isotopologue in the list
	RefListItem<Isotopologue,double>* tope = mix_.contains(iso);
	if (tope == NULL)
	{
		Messenger::warn("Warning: IsotopologueMix does not contain the Isotopologue '%s', so its fraction can't be set.\n", iso->name());
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

// Normalise total relative population to 1.0
void IsotopologueMix::normalise()
{
	double total = totalRelative();
	for (RefListItem<Isotopologue,double>* ri = mix_.first(); ri != NULL; ri = ri->next) ri->data /= total;
}

/*
 * GenericItemBase Implementations
 */

// Return class name
const char* IsotopologueMix::itemClassName()
{
	return "IsotopologueMix";
}

// Write data through specified LineParser
bool IsotopologueMix::write(LineParser& parser)
{
	// Write Species name, integer population, and number of isotopologues in the mix
	if (!parser.writeLineF("'%s'  %i\n", species_->name(), speciesPopulation_, mix_.nItems())) return false;

	// Write Isotopologues
	RefListIterator<Isotopologue,double> mixIterator(mix_);
	while (Isotopologue* top = mixIterator.iterate()) if (!parser.writeLineF("%s  %f\n", top->name(), mixIterator.currentData())) return false;

	return true;
}

// Read data through specified LineParser
bool IsotopologueMix::read(LineParser& parser)
{
	// Read Species name
	if (parser.getArgsDelim(LineParser::UseQuotes) != LineParser::Success) return false;
	for (species_ = List<Species>::masterInstance()->first(); species_ != NULL; species_ = species_->next) if (DUQSys::sameString(species_->name(), parser.argc(0))) break;
	if (species_ == NULL)
	{
		Messenger::error("Failed to find Species '%s' while reading IsotopologueMix.\n", parser.argc(0));
		return false;
	}
	speciesPopulation_ = parser.argi(1);
	int nIso = parser.argi(2);
	mix_.clear();
	for (int n=0; n<nIso; ++n)
	{
		if (parser.getArgsDelim(LineParser::UseQuotes) != LineParser::Success) return false;
		// Search for the named Isotopologue in the Species
		Isotopologue* top = species_->findIsotopologue(parser.argc(0));
		if (!top)
		{
			Messenger::error("Failed to find Isotopologue '%s' for Species '%s' while reading IsotopologueMix.\n", parser.argc(0), species_->name());
			return false;
		}
		mix_.add(top, parser.argd(1));
	}

	return true;
}

/*
 * Parallel Comms
 */

// Broadcast data
bool IsotopologueMix::broadcast(ProcessPool& procPool, int root)
{
#ifdef PARALLEL
	// Broadcast Species info
	if (!List<Species>::hasMasterInstance())
	{
		Messenger::error("Master List<Species> instance has not been set, so IsotopologueMix::broadcast() is not possible.\n");
		return false;
	}
	int speciesIndex;
	if (procPool.poolRank() == root) speciesIndex = List<Species>::masterInstance()->indexOf(species_);
	procPool.broadcast(speciesIndex, root);
	species_ = List<Species>::masterInstance()->item(speciesIndex);

	if (!procPool.broadcast(speciesPopulation_, root)) return false;

	// Isotopologue list (mix_)
	int nIso = mix_.nItems();
	if (!procPool.broadcast(nIso, root)) return false;
	int topIndex;
	if (procPool.poolRank() == root)
	{
		for (int n=0; n<nIso; ++n)
		{
			// Broadcast Isotopologue index data
			topIndex = species_->indexOfIsotopologue(mix_[n]->item);
			if (!procPool.broadcast(topIndex, root)) return false;

			// Broadcast relative population data
			if (!procPool.broadcast(mix_[n]->data, root)) return false;
		}
	}
	else
	{
		mix_.clear();
		double relPop;
		for (int n=0; n<nIso; ++n)
		{
			// Broadcast Isotopologue index data
			if (!procPool.broadcast(topIndex, root)) return false;

			// Broadcast relative population data
			if (!procPool.broadcast(relPop, root)) return false;

			// Add mix data
			mix_.add(species_->isotopologue(topIndex), relPop);
		}
	}
#endif
	return true;
}

