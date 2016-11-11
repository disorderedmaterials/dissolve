/*
	*** Sample Definition
	*** src/classes/sample.cpp
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

#include "classes/sample.h"
#include "classes/species.h"
#include "classes/atomtype.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include <string.h>

// Constructor
Sample::Sample() : ListItem<Sample>()
{
	// Reference F(Q)
	hasReferenceData_ = false;
}

// Copy Constructor
Sample::Sample(const Sample& source)
{
	(*this) = source;
}

// Assignment Operator
void Sample::operator=(const Sample& source)
{
	// Basic Information
	name_ = source.name_;

	// Isotopologue Mix
	isotopologueMixtures_ = source.isotopologueMixtures_;

	// RDF / S(Q) Data
	atomTypes_ = source.atomTypes_;

	// Reference Data
	hasReferenceData_ = source.hasReferenceData_;
	referenceDataFileName_ = source.referenceDataFileName_;
	referenceData_ = source.referenceData_;
}

/*
 * Basic Information
 */

// Set name of Sample
void Sample::setName(const char* name)
{
	 name_ = name;
}

// Return name of Sample
const char* Sample::name() const
{
	return name_.get();
}

/*
 * Species/Isotopologue Definition
 */

// Return whether the Sample contains a mixtures definition for the provided Species
IsotopologueMix* Sample::hasSpeciesIsotopologueMixture(Species* sp) const
{
	/*
	 * This function reconstructs the current RefList of Species/Isotopologue pairs and ensures that
	 * it contains only valid Species and Isotopologue pointers.
	 */
	for (IsotopologueMix* mix = isotopologueMixtures_.first(); mix != NULL; mix = mix->next) if (mix->species() == sp) return mix;
	return NULL;
}

// Update IsotopologueMix List
void Sample::updateIsotopologueMixtures(const List<Species>& species)
{
	/*
	 * This function reconstructs the current List of IsotopologueMix items and ensures that
	 * it contains all valid component Species and Isotopologue pointers.
	 */

	// Construct a temporary RefList, and move all existing RefListItems to it
	List<IsotopologueMix> oldItems;
	IsotopologueMix* mix;
	while (isotopologueMixtures_.last() != NULL)
	{
		mix = isotopologueMixtures_.last();
		isotopologueMixtures_.cut(mix);
		oldItems.own(mix);
	}
	
	// Loop over Species in System, and search for an associated IsotopologueMix in the oldItems list
	for (Species* sp = species.first(); sp != NULL; sp = sp->next)
	{
		for (mix = oldItems.first(); mix != NULL; mix = mix->next) if (mix->species() == sp) break;

		// If we found the existing item, append it to the local list.
		// Otherwise, create a new one with the default (first) Isotopologue.
		if (mix != NULL)
		{
			// Update IsotopologueMix, and check that it still contains something...
			// If there are no isotopologues left in the mix, try to add one
			mix->update();
			if (mix->nIsotopologues() == 0) mix->addNextIsotopologue();
			
			// If we get here, its still valid, so store it
			oldItems.cut(mix);
			isotopologueMixtures_.own(mix);
		}
		else
		{
			mix = isotopologueMixtures_.add();
			mix->setSpecies(sp);
		}
	}
}

// Add Isotopologue for Species
bool Sample::addIsotopologueToMixture(Species* sp, Isotopologue *iso, double relPop)
{
	// Check that the Species is in the list...
	IsotopologueMix* mix = hasSpeciesIsotopologueMixture(sp);
	if (mix == NULL)
	{
		Messenger::print("Warning: Sample '%s' contains no IsotopologueMix definition for Species '%s'.\n", name_.get(), sp->name());
		return false;
	}

	// Check current number of Isotopologues defined against total available
	if (sp->nIsotopologues() == mix->nIsotopologues())
	{
		Messenger::print("Can't add a new Isotopologue definition for Species '%s' in Sample '%s' since there are no unused Isotopologue definitions left.\n", sp->name(), name_.get());
		return false;
	}

	// Was a specific Isotopologue provided?
	if (iso == NULL)
	{
		// Add next available Isotopologue to mixture
		if (!mix->addNextIsotopologue()) return false;
	}
	else if (!mix->addIsotopologue(iso, relPop))
	{
		Messenger::error("Failed to add Isotopologue to Sample '%s'.\n", name_.get());
		return false;
	}
	
	return true;
}

// Return first IsotopologueMix
IsotopologueMix *Sample::isotopologueMixtures() const
{
	return isotopologueMixtures_.first();
}

// Return nth IsotopologueMix
IsotopologueMix *Sample::isotopologueMixture(int n)
{
	return isotopologueMixtures_[n];
}

// Assign default (first) Isotopologues for all Species
void Sample::assignDefaultIsotopes()
{
	for (IsotopologueMix* mix = isotopologueMixtures_.first(); mix != NULL; mix = mix->next)
	{
		if (mix->nIsotopologues() != 0) continue;
		if (mix->species()->nIsotopologues() == 0) continue;
		mix->addNextIsotopologue();
	}
}

// Create type list
bool Sample::createTypeList(const List<Species>& allSpecies, const List<AtomType>& masterIndex)
{
	// Loop over Samples and go through Isotopologues in each mixture
	atomTypes_.clear();
	Isotope* iso;

	Messenger::print("--> Generating AtomType/Isotope index...\n");
	// Simultaneous loop over defined Species and IsotopologueMixtures (which are directly related)
	Species* refSp = allSpecies.first();
	for (IsotopologueMix* mix = isotopologueMixtures_.first(); mix != NULL; mix = mix->next, refSp = refSp->next)
	{
		// Sanity check
		if (mix->species() != refSp)
		{
			Messenger::error("Species referred to in mixture in Sample '%s' does not correspond to that in the main Species list.\n", mix->species()->name());
			return false;
		}

		// We must now loop over the Isotopologues in the mixture
		for (RefListItem<Isotopologue,double>* tope = mix->isotopologues(); tope != NULL; tope = tope->next)
		{
			// Loop over Atoms in the Species, searching for the AtomType/Isotope entry in the isotopes list of the Isotopologue
			for (SpeciesAtom* i = refSp->atoms(); i != NULL; i = i->next)
			{
				iso = tope->item->atomTypeIsotope(i->atomType());
				atomTypes_.add(i->atomType(), iso);
			}
		}
	}

	// Calculate fractional populations
	atomTypes_.finalise();

	// Set master type indices
	for (AtomTypeData* atd = atomTypes_.first(); atd != NULL; atd = atd->next)
	{
		// Find entry in the master index which contains the AtomType of 'at1'
		int id = masterIndex.indexOf(atd->atomType());
		if (id < 0)
		{
			Messenger::print("INTERNAL_ERROR - Couldn't find entry for first AtomType '%s' in masterIndex.\n", atd->name());
			return false;
		}
		atd->setMasterIndex(id);
	}

	return true;
}

/*
 * Reference Data
 */

// Return whether reference data exists
bool Sample::hasReferenceData()
{
	return hasReferenceData_;
}

// Load reference F(Q) data
bool Sample::loadReferenceData(const char* filename)
{
	// Check that the specified file actually exists...
	if (!DUQSys::fileExists(filename))
	{
		Messenger::error("Sample reference data '%s' not found.\n", filename);
		return false;
	}

	referenceDataFileName_ = filename;
	referenceData_.clear();

	// Open file first...
	LineParser parser;
	parser.openInput(filename);
	if (!parser.isFileGoodForReading())
	{
		Messenger::error("Couldn't open datafile '%s'.\n", filename);
		return false;
	}

	Messenger::print("--> Loading reference F(Q) data from '%s'...\n", referenceDataFileName_.get());

	// Read data, assuming that column 0 = Q and column 1 = F(Q)...
	bool result = true;
	while (!parser.eofOrBlank())
	{
		if (parser.getArgsDelim(LineParser::SkipBlanks+LineParser::StripComments) == 1)
		{
			Messenger::error("File error while reading '%s'.\n", referenceDataFileName_.get());
			result = false;
			break;
		}
		referenceData_.addPoint(parser.argd(0), parser.argd(1));
	}

	// Tidy up
	parser.closeFiles();
	hasReferenceData_ = result;

	return result;
}

// Return reference data filename (if any)
Dnchar& Sample::referenceDataFileName()
{
	return referenceDataFileName_;
}

// Return reference data
Data2D& Sample::referenceData()
{
	return referenceData_;
}

// Return calculated data
Data2D& Sample::calculatedData()
{
	return calculatedData_;
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool Sample::broadcast(ProcessPool& procPool, const List<Species>& species)
{
#ifdef PARALLEL
	int index, topeCount;
	double relPop;
	IsotopologueMix* iso;

	// Send name
	if (!procPool.broadcast(name_)) return false;

	// Mixture information
	// Component/Species RefList will have already been constructed in DUQ::addSample(), so just update constituent Isotopologue
	for (iso = isotopologueMixtures_.first(); iso != NULL; iso = iso->next)
	{
		// Master needs to determine Species index
		if (procPool.isMaster()) index = species.indexOf(iso->species());
		if (!procPool.broadcast(&index, 1)) return false;
		iso->setSpecies(species.item(index));

		// Now sent number of isotopes in mixture
		topeCount = iso->nIsotopologues();
		if (!procPool.broadcast(&topeCount, 1)) return false;
		
		if (procPool.isMaster()) for (RefListItem<Isotopologue,double>* ri = iso->isotopologues(); ri != NULL; ri = ri->next)
		{
			// Get Isotopologue index from Species
			index = iso->species()->indexOfIsotopologue(ri->item);
			if (!procPool.broadcast(&index, 1)) return false;
			// Send relative population
			relPop = ri->data;
			if (!procPool.broadcast(&relPop, 1)) return false;
		}
		else
		{
			for (int m = 0; m<topeCount; ++m)
			{
				// Receive Isotopologue index in associated Species
				if (!procPool.broadcast(&index, 1)) return false;
				// Receive relative population
				if (!procPool.broadcast(&relPop, 1)) return false;
				// Add new mixture component
				iso->addIsotopologue(iso->species()->isotopologue(index), relPop);
			}
		}
	}

	// Reference data
	if (!procPool.broadcast(&hasReferenceData_, 1)) return false;
	if (!referenceData_.broadcast(procPool)) return false;
	if (!procPool.broadcast(referenceDataFileName_)) return false;
#endif
	return true;
}
