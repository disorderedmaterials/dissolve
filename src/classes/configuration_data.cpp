/*
	*** Configuration - Reference Data
	*** src/classes/configuration_data.cpp
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

#include "classes/configuration.h"
#include "classes/box.h"
#include "classes/species.h"
#include <string.h>

// Search for Sample by name
Sample* Configuration::findReferenceSample(const char* name) const
{
	for (Sample* sample = referenceSamples_.first(); sample != NULL; sample = sample->next) if (strcmp(name,sample->name()) == 0) return sample;

	return NULL;
}

// Return first referenced Sample
bool Configuration::addReferenceSample(Sample* source)
{
	// Search for the sample (by name) to see if we have added it already
	if (findReferenceSample(source->name()))
	{
		Messenger::error("Configuration '%s' already references Sample '%s'.\n", name(), source->name());
		return false;
	}

	Sample* newSample = referenceSamples_.add();
	(*newSample) = (*source);

	return newSample;
}

// Return first referenced Sample
Sample* Configuration::referenceSamples()
{
	return referenceSamples_.first();
}

// Set up Sample data for Configuration
bool Configuration::setupReferenceSamples()
{
	// Loop over Samples and initialise as necessary
	double rho = atomicDensity();
	for (Sample* sam = referenceSamples_.first(); sam != NULL; sam = sam->next)
	{
		// Print out some useful info
		Messenger::print("--> Reference Sample: '%s' (in Configuration '%s')\n", sam->name(), name());
		for (IsotopologueMix* mix = sam->isotopologueMixtures(); mix != NULL; mix = mix->next)
		{
			double totalRelative = mix->totalRelative();
			for (RefListItem<Isotopologue,double>* tope = mix->isotopologues(); tope != NULL; tope = tope->next)
			{
				if (tope == mix->isotopologues()) Messenger::print("       %-15s  %-15s  %8.3f\n", mix->species()->name(), tope->item->name(), tope->data / totalRelative);
				else Messenger::print("                        %-15s  %8.3f\n", tope->item->name(), tope->data / totalRelative);
			}
		}
		
		// Create AtomType index for Sample
		if (!sam->createTypeIndex(usedSpecies_, multiplier_, nAtoms_, usedAtomTypes_)) return false;

		// Finalise reference data
		if (!sam->finaliseReferenceData()) return false;

		// Setup pair correlation arrays
		if (!sam->setupPairCorrelations(box_->volume(), rdfRange_, rdfBinWidth_, boxNormalisation_, rho)) return false;

		Messenger::print("\n");
	}

	return true;
}

/*
 * Calculate the current RMSE between reference and calculated data.
 */
double Configuration::rmse()
{
	// Calculate partials
	if (coordinateIndex_ != partialsIndex_)
	{
		Messenger::error("Partials data is out of date - rmse() returning zero...\n");
		return 0.0;
	}

	// Calculate RMSE over all samples
	double rmse = 0.0, sampleRMSE;
// 	for (Sample* sam = samples_.first(); sam != NULL; sam = sam->next)
// 	{
// 		if (!sam->hasReferenceData()) continue;
// 		
// 		sampleRMSE = sam->referenceRMSE(rmseDeltaQ_);
// 		Messenger::print("--> Sample '%s' has RMSE %12.5e barns/sr/atom.\n", sam->name(), sampleRMSE);
// 		rmse += sampleRMSE;
// 	}

	Messenger::print("--> Total RMSE over all Samples is %12.5e barns/sr/atom.\n", rmse);
// 	return true;
}