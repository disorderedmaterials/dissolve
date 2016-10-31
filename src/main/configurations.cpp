/*
	*** dUQ - Configurations
	*** src/main/configurations.cpp
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
#include <string.h>

// Add new Configuration
Configuration* DUQ::addConfiguration()
{
	return configurations_.add();
}

// Find configuration by name
Configuration* DUQ::findConfiguration(const char* name) const
{
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next) if (strcmp(name, cfg->name()) == 0) return cfg;

	return NULL;
}

// Calculate Box normalisations
bool DUQ::calculateBoxNormalisations()
{
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
	{
		// Does a box normalisation already exist (i.e. has been loaded)
		// Create/load Box normalisation array
		if (!boxNormalisationFileName_.isEmpty())
		{
			if (boxNormalisation_.load(boxNormalisationFileName_))
			{
				Messenger::print("--> Successfully loaded box normalisation data from file '%s'.\n", boxNormalisationFileName_.get());
				boxNormalisation_.interpolate();
			}
			else Messenger::print("--> Couldn't load Box normalisation data - it will be calculated.\n");
		}
		if (boxNormalisation_.nPoints() <= 1)
		{
			// Only calculate if RDF range is greater than the inscribed sphere radius
			if (rdfRange_ <= inscribedSphereRadius)
			{
				Messenger::print("--> No need to calculate Box normalisation array since rdfRange is within periodic range.\n");
				boxNormalisation_.clear();
				double x = rdfBinWidth_*0.5;
				while (x < rdfRange_)
				{
					boxNormalisation_.addPoint(x, 1.0);
					x += rdfBinWidth_;
				}
				boxNormalisation_.interpolate();
			}
			else
			{
				Messenger::print("--> Calculating box normalisation array for RDFs...\n");
				if (!box()->calculateRDFNormalisation(boxNormalisation_, rdfRange_, rdfBinWidth_, boxNormalisationNPoints)) return false;
			}
		}
	}
}
