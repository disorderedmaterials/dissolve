/*
	*** dUQ - Configurations
	*** src/main/configurations.cpp
	Copyright T. Youngs 2012-2017

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

// Return first Configuration in list
Configuration* DUQ::configurations()
{
	return configurations_.first();
}

// Find configuration by name
Configuration* DUQ::findConfiguration(const char* name, bool useNiceName) const
{
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next) if (DUQSys::sameString(name, useNiceName ? cfg->niceName() : cfg->name())) return cfg;

	return NULL;
}
