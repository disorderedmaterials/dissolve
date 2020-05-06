/*
	*** Isotopologue Collection
	*** src/classes/isotopologuecollection.cpp
	Copyright T. Youngs 2012-2020

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

#include "classes/isotopologuecollection.h"
#include "base/lineparser.h"
#include "base/processpool.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include <algorithm>

IsotopologueCollection::IsotopologueCollection() { clear(); }

IsotopologueCollection::~IsotopologueCollection() {}

/*
 * Sets
 */

// Remove any sets from the collection that are empty
void IsotopologueCollection::pruneEmptySets()
{
	std::remove_if(isotopologueSets_.begin(), isotopologueSets_.end(), [&](IsotopologueSet &set) {
		return set.nIsotopologues() == 0;
	});
}

// Clear all existing data
void IsotopologueCollection::clear() { isotopologueSets_.clear(); }

// Add Isotopologue weight for the specified Configuration / Species
void IsotopologueCollection::add(Configuration *cfg, Isotopologue *iso, double relativeWeight)
{
	printf("TRYING TO ADD ISO %s TO CFG %s...\n", iso->name(), cfg->name());
	// Check if a set already exists for this Configuration
	auto it = std::find_if(isotopologueSets_.begin(), isotopologueSets_.end(), [&](IsotopologueSet &set) {
		return set.configuration() == cfg;
	});
	if (it == isotopologueSets_.end())
	{
		isotopologueSets_.emplace_back(this, cfg);
		isotopologueSets_.back().add(iso, relativeWeight);
	}
	else
		it->add(iso, relativeWeight);
}

// Remove the specified set from the collection
void IsotopologueCollection::remove(IsotopologueSet *set)
{
	std::remove_if(isotopologueSets_.begin(), isotopologueSets_.end(), [&](IsotopologueSet &data) {
		return &data == set;
	});
}

// Remove the Configuration from the collection
void IsotopologueCollection::remove(Configuration *cfg)
{
	std::remove_if(isotopologueSets_.begin(), isotopologueSets_.end(), [&](IsotopologueSet &set) {
		return set.configuration() == cfg;
	});
}

// Remove the Species from the specified set
void IsotopologueCollection::remove(IsotopologueSet *set, Species *sp)
{
	set->remove(sp);

	pruneEmptySets();
}

// Remove the IsotopologueWeight from the specified set
void IsotopologueCollection::remove(IsotopologueSet *set, IsotopologueWeight *isoWeight)
{
	set->remove(isoWeight);

	pruneEmptySets();
}

// Remove any occurrences of the specified Species from the collection
void IsotopologueCollection::remove(Species *sp)
{
	for (auto set : isotopologueSets_)
		set.remove(sp);

	pruneEmptySets();
}

// Remove any occurrences of the specified Isotopologue from the collection
void IsotopologueCollection::remove(Isotopologue *iso)
{
	for (auto set : isotopologueSets_)
		set.remove(iso);

	pruneEmptySets();
}

// Return defined sets
const std::vector<IsotopologueSet> &IsotopologueCollection::isotopologueSets() const { return isotopologueSets_; }

// Return whether a set exists for the supplied Configuration
bool IsotopologueCollection::contains(const Configuration *cfg) const
{
	auto it = std::find_if(isotopologueSets_.cbegin(), isotopologueSets_.cend(), [&](const IsotopologueSet &set) {
		return set.configuration() == cfg;
	});
	return (it != isotopologueSets_.end());
}

// Return IsotopologueSet for the specified Configuration
optional<const IsotopologueSet> IsotopologueCollection::getIsotopologueSet(const Configuration *cfg) const
{
	auto it = std::find_if(isotopologueSets_.cbegin(), isotopologueSets_.cend(), [&](const IsotopologueSet &set) {
		return set.configuration() == cfg;
	});

	return std::make_tuple(*it, it == isotopologueSets_.end());
}

// Return whether the Species has a defined set of isotopologues in the specified Configuration
bool IsotopologueCollection::contains(const Configuration *cfg, const Species *sp) const
{
	auto it = std::find_if(isotopologueSets_.cbegin(), isotopologueSets_.cend(), [&](const IsotopologueSet &set) {
		return set.configuration() == cfg;
	});

	return (it != isotopologueSets_.end() ? it->contains(sp) : false);
}

// Return Isotopologues for the Species in the specified Configuration
optional<const Isotopologues> IsotopologueCollection::getIsotopologues(const Configuration *cfg, const Species *sp) const
{
	auto it = std::find_if(isotopologueSets_.cbegin(), isotopologueSets_.cend(), [&](const IsotopologueSet &set) {
		return set.configuration() == cfg;
	});

	if (it != isotopologueSets_.end())
		return it->getIsotopologues(sp);

	static const Isotopologues dummyTopes;
	return std::make_tuple(dummyTopes, true);
}

// Complete the collection by making sure it contains every Species in every Configuration in the supplied list
void IsotopologueCollection::complete(const RefList<Configuration> &configurations)
{
	for (Configuration *cfg : configurations)
	{
		// Retrieve / create a set for this Configuration
		auto it = std::find_if(isotopologueSets_.begin(), isotopologueSets_.end(), [&](IsotopologueSet &set) {
			return set.configuration() == cfg;
		});
		IsotopologueSet* setForCfg = nullptr;
		if (it == isotopologueSets_.end())
		{
			isotopologueSets_.emplace_back(this, cfg);
			setForCfg = &isotopologueSets_.back();
		}
		else
			setForCfg = &(*it);

		// Loop over Species in the Configuration
		ListIterator<SpeciesInfo> spInfoIterator(cfg->usedSpecies());
		while (SpeciesInfo *spInfo = spInfoIterator.iterate())
		{
			// If the Species already exists in our set, nothing more to do...
			if (it->contains(spInfo->species()))
				continue;

			// Add the natural isotopologue for this Species
			add(cfg, spInfo->species()->naturalIsotopologue(), 1.0);
		}
	}
}

/*
 * GenericItemBase Implementations
 */

// Return class name
const char *IsotopologueCollection::itemClassName() { return "IsotopologueCollection"; }

// Read data through specified LineParser
bool IsotopologueCollection::read(LineParser &parser, const CoreData &coreData)
{
	clear();

	// Read in number of Configurations in the collection
	const int nConfigurations = parser.argi(0);

	for (int n = 0; n < nConfigurations; ++n)
	{
		// Add a new isotopologue set and read it
		isotopologueSets_.emplace_back(this);
		if (!isotopologueSets_.back().read(parser, coreData))
			return false;
	}

	return true;
}

// Write data through specified LineParser
bool IsotopologueCollection::write(LineParser &parser)
{
	// Write number of Configurations in the collection
	if (!parser.writeLineF("%i\n", isotopologueSets_.size()))
		return false;

	// Write details for each set of Isotopologues
	for (auto set : isotopologueSets_)
		if (!set.write(parser))
			return false;

	return true;
}
