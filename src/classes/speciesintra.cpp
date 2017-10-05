/*
	*** SpeciesIntra Definition
	*** src/classes/speciesintra.cpp
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

#include "classes/speciesintra.h"
#include "base/messenger.h"

// Constructor
SpeciesIntra::SpeciesIntra()
{
	parent_ = NULL;
	for (int n=0; n<MAXINTRAPARAMS; ++n) parameters_[n] = 0.0;
}

// Destructor
SpeciesIntra::~SpeciesIntra()
{
}

/*
 * Basic Data
 */

// Set parent Species
void SpeciesIntra::setParent(Species* parent)
{
	parent_ = parent;
}

// Return parent Species
Species* SpeciesIntra::parent() const
{
	return parent_;
}

// Set name for interaction (if relevant)
void SpeciesIntra::setName(const char* name)
{
	name_ = name;
}

// Return name for interaction
const char* SpeciesIntra::name()
{
	return name_.get();
}

/*
 * Interaction Parameters
 */

// Set nth parameter
void SpeciesIntra::setParameter(int id, double value)
{
#ifdef CHECKS
	if ((id < 0) || (id >= MAXINTRAPARAMS))
	{
		Messenger::error("Tried to add a parameter to a SpeciesIntra definition, but the index is out of range (%i vs %i parameters max).\n", id, MAXINTRAPARAMS);
		return;
	}
#endif
	parameters_[id] = value;
}

// Return nth parameter
double SpeciesIntra::parameter(int id) const
{
#ifdef CHECKS
	if ((id < 0) || (id >= MAXINTRAPARAMS))
	{
		Messenger::error("Tried to return a parameter from a SpeciesIntra definition, but the index is out of range (%i vs %i parameters max).\n", id, MAXINTRAPARAMS);
		return 0.0;
	}
#endif
	return parameters_[id];
}
