/*
	*** dUQ - Samples
	*** src/main/samples.cpp
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

#include "main/duq.h"
#include "classes/box.h"
#include "classes/species.h"
#include "base/sysfunc.h"
#include <string.h>

// Update current Samples
void DUQ::updateSamples()
{
	// Easy check - are there any Species defined?
	if (species_.nItems() == 0) samples_.clear();

	// Check all Species / Isotopologue definitions
	for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next) sam->updateIsotopologueMixtures(species_);
}

// Add new Sample
Sample* DUQ::addSample(const char* baseName)
{
	Sample* sample = samples_.add();
	sample->setName(baseName);
	sample->updateIsotopologueMixtures(species_);

	return sample;
}

// Remove Sample
void DUQ::removeSample(Sample* sample)
{
	if (samples_.contains(sample))
	{
		samples_.remove(sample);
	}
	else Messenger::print("Warning: Can't remove Sample '%s' from the list, since it isn't in this list.\n", sample->name());
}

// Return first Sample
Sample* DUQ::samples() const
{
	return samples_.first();
}

// Return nth Sample
Sample* DUQ::sample(int n)
{
	return samples_[n];
}

// Search for Sample by name
Sample* DUQ::findSample(const char* name) const
{
	for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next) if (strcmp(name, sam->name()) == 0) return sam;
	return NULL;
}

// Setup Samples
bool DUQ::setupSamples()
{
	/*
	 * Functions performed by this routine:
	 *  1)	Construct list of AtomTypes used by the Isotopologues referenced in this sample
	 *  2)	Finalise any reference data
	 *  3)	Initialise Modules assigned to this Sample
	 * 
	 * The AtomTypeList will contain a list of all AtomTypes used by the Isotopologues referenced by the sample, but will *not* contain
	 * any population information (since this is determined only by a Configuration).  The Sample is therefore a simple reference
	 * container that holds some kind of data with which to compare against. The AtomTypeList is used by various Modules (e.g. StructureFactor)
	 * in order to calculate, from reference Configurations, the correctly-weighted partials / F(Q).
	 */

	// Loop over Samples
	for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next)
	{
		// Print out some useful info
		Messenger::print("--> Sample: '%s'\n", sam->name());
		for (IsotopologueMix* mix = sam->isotopologueMixtures(); mix != NULL; mix = mix->next)
		{
			for (RefListItem<Isotopologue,double>* tope = mix->isotopologues(); tope != NULL; tope = tope->next)
			{
				if (tope == mix->isotopologues()) Messenger::print("       %-15s  %-15s\n", mix->species()->name(), tope->item->name());
				else Messenger::print("                        %-15s\n", tope->item->name());
			}
		}

		// Create AtomType list for Sample
		if (!sam->createTypeList(species_, atomTypes_)) return false;

		// MODULES
		// TODO

		Messenger::print("\n");
	}

	return true;
}
