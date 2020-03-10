/*
	*** Dissolve - Object Management
	*** src/main/objects.cpp
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

#include "main/dissolve.h"

// Remove all references to the specified Configuration
void Dissolve::removeReferencesTo(Configuration* cfg)
{
	// Remove references in keywords
	KeywordBase::objectNoLongerValid<Configuration>(cfg);

	// Remove references (targets) in Modules
	for (Module* module : moduleInstances_) if (module->isTargetConfiguration(cfg)) module->removeTargetConfiguration(cfg);
}

// Remove all references to the specified Module
void Dissolve::removeReferencesTo(Module* module)
{
	// Remove references in keywords
	KeywordBase::objectNoLongerValid<Module>(module);
}

// Remove all references to the specified Species
void Dissolve::removeReferencesTo(Species* sp)
{
	// Remove references in keywords
	KeywordBase::objectNoLongerValid<Species>(sp);

	// Check Configurations - if the Species was used, we must clear the configuration contents
	ListIterator<Configuration> configIterator(configurations());
	while (Configuration* cfg = configIterator.iterate()) if (cfg->hasUsedSpecies(sp)) cfg->empty();
}

// Remove all references to the specified SpeciesSite
void Dissolve::removeReferencesTo(SpeciesSite* site)
{
	// Remove references in keywords
	KeywordBase::objectNoLongerValid<SpeciesSite>(site);
}
