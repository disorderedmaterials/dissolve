/*
	*** dUQ - Samples
	*** src/main/samples.cpp
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
Sample *DUQ::addSample(const char* baseName)
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
	else msg.print("Warning: Can't remove Sample '%s' from the list, since it isn't in this list.\n", sample->name());
}

// Return first Sample
Sample *DUQ::samples() const
{
	return samples_.first();
}

// Return nth Sample
Sample *DUQ::sample(int n)
{
	return samples_[n];
}

// Search for Sample by name
Sample* DUQ::findSample(const char* name) const
{
	for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next) if (strcmp(name, sam->name()) == 0) return sam;
	return NULL;
}
