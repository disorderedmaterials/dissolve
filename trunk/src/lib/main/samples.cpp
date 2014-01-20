/*
	*** dUQ - Samples
	*** src/lib/main/samples.cpp
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
#include "main/flags.h"
#include "classes/box.h"
#include "classes/species.h"
#include "base/sysfunc.h"
#include <string.h>

/*
// Samples
*/

/*!
 * \brief Update current Samples
 * \details After removal of a Species or removal/addition of an Isotopologue, any existing Samples must be updated to
 * reflect those changes. In particular, if a Sample becomes empty through the removal of all Species, the
 * Sample itself is deleted.
 */
void DUQ::updateSamples()
{
	// Easy check - are there any Species defined?
	if (species_.nItems() == 0) samples_.clear();

	// Check all Species / Isotopologue definitions
	for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next) sam->updateIsotopologueMixtures(species_);
}

/*!
 * \brief Add Sample
 * \details Add a new Sample to the current System.
 */
Sample *DUQ::addSample(const char* baseName)
{
	Sample* sample = samples_.add();
	sample->setName(baseName);
	sample->updateIsotopologueMixtures(species_);
	
	Flags::wave(Flags::SamplesChanged);
	
	return sample;
}

/*!
 * \brief Remove Sample
 */
void DUQ::removeSample(Sample* sample)
{
	if (samples_.contains(sample))
	{
		samples_.remove(sample);
		
		Flags::wave(Flags::SamplesChanged);
	}
	else msg.print("Warning: Can't remove Sample '%s' from the list, since it isn't in this list.\n", sample->name());
}

/*!
 * \brief Return first Sample
 */
Sample *DUQ::samples() const
{
	return samples_.first();
}

/*!
 * \brief Return nth Sample
 */
Sample *DUQ::sample(int n)
{
	return samples_[n];
}

/*!
 * \brief Generate unique Sample name with base name provided
 */
const char* DUQ::uniqueSampleName(const char* base, Sample* exclude) const
{
	static Dnchar uniqueName;
	Dnchar baseName = base;
	Sample* s;
	int highest = -1;
	
	if (baseName.isEmpty()) baseName = "Unnamed";
	
	// Find all existing names which are the same as 'baseName' up to the first '_', and get the highest appended number
	for (s = samples_.first(); s != NULL; s = s->next)
	{
		if (s == exclude) continue;
		if (strcmp(baseName, s->name()) == 0) highest = 0;
		else if (strcmp(baseName,beforeLastChar(s->name(),'_')) == 0) highest = atoi(afterLastChar(s->name(), '_'));
	}
	if (highest > -1) uniqueName.sprintf("%s_%i", baseName.get(), ++highest);
	else uniqueName = baseName;
	
	return uniqueName;
}

/*!
 * \brief Set up Sample data
 */
bool DUQ::setupSamples()
{
	// Loop over Samples and initialise as necessary
	double rho = atomicDensity();
	for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next)
	{
		// Print out some useful info
		msg.print("--> Sample: '%s'\n", sam->name());
		for (IsotopologueMix* mix = sam->isotopologueMixtures(); mix != NULL; mix = mix->next)
		{
			double totalRelative = mix->totalRelative();
			for (RefListItem<Isotopologue,double>* tope = mix->isotopologues(); tope != NULL; tope = tope->next)
			{
				if (tope == mix->isotopologues()) msg.print("       %-15s  %-15s  %8.3f\n", mix->species()->name(), tope->item->name(), tope->data / totalRelative);
				else msg.print("                        %-15s  %8.3f\n", tope->item->name(), tope->data / totalRelative);
			}
		}
		
		// Create AtomType index for Sample
		if (!sam->createTypeIndex(species_, multiplier_, configuration_.nAtoms(), typeIndex_)) return false;

		// Finalise reference data
		if (!sam->finaliseReferenceData()) return false;

		// Setup pair correlation arrays
		if (!sam->setupPairCorrelations(configuration_.box()->volume(), rdfRange_, rdfBinWidth_, rdfExtensionLimit_, boxNormalisation_, rho)) return false;

		msg.print("\n");
	}
	return true;
}
