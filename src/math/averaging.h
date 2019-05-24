/*
	*** Averaging
	*** src/math/average.h
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

#ifndef DISSOLVE_AVERAGE_H
#define DISSOLVE_AVERAGE_H

#include "base/enumoptions.h"
#include "base/genericlist.h"
#include "templates/genericlisthelper.h"

// Forward Declarations
/* none */

// Data Averaging
class Averaging
{
	public:
	// Averaging Schemes
	enum AveragingScheme
	{
		LinearAveraging, ExponentialAveraging, nAveragingSchemes
	};
	// Return enum options for AveragingScheme
	static EnumOptions<Averaging::AveragingScheme> averagingSchemes();

	public:
	// Perform averaging of named data
	template <class T> static bool average(GenericList& moduleData, const char* name, const char* prefix, int nSetsInAverage, AveragingScheme averagingScheme)
	{
		const int expDecay = 0.7;

		// Find the 'root' data of type T, which should currently contain the most recently-calculated data
		if (!moduleData.contains(name, prefix))
		{
			Messenger::error("Couldn't find root data '%s' (prefix = '%s') in order to perform averaging.\n", name, prefix);
			return false;
		}
		T& currentData = GenericListHelper<T>::retrieve(moduleData, name, prefix);

		// Establish how many stored datasets we have
		int nStored = 0;
		for (nStored = 0; nStored < nSetsInAverage; ++nStored) if (!moduleData.contains(CharString("%s_%i", name, nStored+1), prefix)) break;
		Messenger::print("Average requested over %i datsets - %i available in module data (%i max).\n", nSetsInAverage, nStored, nSetsInAverage-1);

		// Remove the oldest dataset if it exists, and shuffle the others down
		if (nStored == nSetsInAverage)
		{
			moduleData.remove(CharString("%s_%i", name, nStored), prefix);
			--nStored;
		}
		for (int n=nStored; n>0; --n) moduleData.rename(CharString("%s_%i", name, n), prefix, CharString("%s_%i", name, n+1), prefix);

		// Store the current T as the earliest data (1)
		T& recentData = GenericListHelper<T>::realise(moduleData, CharString("%s_1", name), prefix, GenericItem::InRestartFileFlag);
		recentData = currentData;
		++nStored;

		// Calculate normalisation
		double normalisation = 0.0;
		if (averagingScheme == Averaging::LinearAveraging) normalisation = nStored;
		else if (averagingScheme == Averaging::ExponentialAveraging) normalisation = (1.0 - pow(expDecay,nStored)) / (1.0 - expDecay); 

		// Perform averaging of the datsets that we have
		currentData.reset();
		double weight = 1.0;
		for (int n=0; n<nStored; ++n)
		{
			// Get a copy of the (n+1)'th dataset
			T data = GenericListHelper<T>::value(moduleData, CharString("%s_%i", name, n+1), prefix);

			// Determine the weighting factor
			if (averagingScheme == Averaging::LinearAveraging) weight = 1.0 / normalisation;
			else if (averagingScheme == Averaging::ExponentialAveraging) weight = pow(expDecay, n) / normalisation;

			// Weight the data
			data *= weight;

			// Sum in to the average
			currentData += data;
		}

		return true;
	};
};

#endif
