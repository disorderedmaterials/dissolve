/*
	*** Isotopologue Reference
	*** src/classes/isotopologuereference.cpp
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

#include "classes/isotopologuereference.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/processpool.h"

// Constructor
IsotopologueReference::IsotopologueReference() : ListItem<IsotopologueReference>()
{
	configuration_ = NULL;
	species_ = NULL;
	isotopologue_ = NULL;
}

// Destructor
IsotopologueReference::~IsotopologueReference()
{
}

/*
 * Data
 */

// Set all pointers
void IsotopologueReference::set(Configuration* cfg, Species* sp, Isotopologue* iso, double weight)
{
	configuration_ = cfg;
	species_ = sp;
	isotopologue_ = iso;
	weight_ = weight;
}

// Set Configuration in which the Isotopologue is used
void IsotopologueReference::setConfiguration(Configuration* cfg)
{
	configuration_ = cfg;
}

// Return Configuration in which the Isotopologue is used
Configuration* IsotopologueReference::configuration()
{
	return configuration_;
}

// Set Species to which Isotopologue is related
void IsotopologueReference::setSpecies(Species* sp)
{
	species_ = sp;
}

// Return Species to which Isotopologue is related
Species* IsotopologueReference::species()
{
	return species_;
}

// Isotopologue for Species
void IsotopologueReference::setIsotopologue(Isotopologue* iso)
{
	isotopologue_ = iso;
}

// Return Isotopologue for Species
Isotopologue* IsotopologueReference::isotopologue()
{
	return isotopologue_;
}

// Set weight
void IsotopologueReference::setWeight(double weight)
{
	weight_ = weight;
}

// Return weight
double IsotopologueReference::weight()
{
	return weight_;
}

// Return whether the supplied Configuration and Species match
bool IsotopologueReference::matches(Configuration* cfg, Species* sp) const
{
	return ((configuration_ == cfg) && (species_ == sp));
}

/*
 * GenericItemBase Implementations
 */

// Return class name
const char* IsotopologueReference::itemClassName()
{
	return "IsotopologueReference";
}

// Read data through specified LineParser
bool IsotopologueReference::read(LineParser& parser, const CoreData& coreData)
{
	// Find target Configuration (first argument)
	configuration_ = coreData.findConfiguration(parser.argc(0));
	if (!configuration_)
	{
		Messenger::error("Error defining Isotopologue reference - no Configuration named '%s' exists.\n", parser.argc(0));
		return false;
	}

	// Find specified Species (second argument) - must be present in the target Configuration
	species_ = coreData.findSpecies(parser.argc(1));
	if (!species_) return Messenger::error("Error defining Isotopologue reference - no Species named '%s' exists.\n", parser.argc(1));
	if (!configuration_->hasUsedSpecies(species_))return Messenger::error("Error defining Isotopologue reference - Species '%s' is not present in Configuration '%s'.\n", species_->name(), configuration_->name());

	// Finally, locate isotopologue definition for species
	isotopologue_ = species_->findIsotopologue(parser.argc(2));
	if (!isotopologue_) return Messenger::error("Error defining Isotopologue reference - no Isotopologue named '%s' exists for Species '%s'.\n", parser.argc(2), species_->name());

	// Set weight
	weight_ = parser.argd(3);

	return true;
}

// Write data through specified LineParser
bool IsotopologueReference::write(LineParser& parser)
{
	return parser.writeLineF("'%s'  '%s'  '%s'  %f", configuration_->name(), species_->name(), isotopologue_->name(), weight_);
}

/*
 * Parallel Comms
 */

// Broadcast data
bool IsotopologueReference::broadcast(ProcessPool& procPool, const int root, const CoreData& coreData)
{
	return false;
}

// Check item equality
bool IsotopologueReference::equality(ProcessPool& procPool)
{
	return false;
}
