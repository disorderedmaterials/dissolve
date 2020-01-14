/*
	*** MasterIntra Definition
	*** src/classes/masterintra.cpp
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

#include "classes/masterintra.h"
#include "classes/atom.h"
#include "base/messenger.h"

// Constructor
MasterIntra::MasterIntra() : SpeciesIntra(), ListItem<MasterIntra>()
{
	parent_ = NULL;
	masterParameters_ = NULL;
	for (int n=0; n<MAXINTRAPARAMS; ++n) parameters_[n] = 0.0;
}

// Destructor
MasterIntra::~MasterIntra()
{
}

/*
 * Basic Data
 */

// Set name for interaction (if relevant)
void MasterIntra::setName(const char* name)
{
	name_ = name;
}

// Return name for interaction
const char* MasterIntra::name() const
{
	return name_.get();
}

// Set type of interaction
void MasterIntra::setType(SpeciesIntra::InteractionType type)
{
	type_ = type;
}

/*
 * Interaction Parameters
 */

// Set up any necessary parameters
void MasterIntra::setUp()
{
}

// Calculate and return fundamental frequency for the interaction
double MasterIntra::fundamentalFrequency(double reducedMass) const
{
	Messenger::warn("A MasterIntra can't return a fundamental frequency, so don't ask for one!\n");
	return 0.0;
}

// Return type of this interaction
SpeciesIntra::InteractionType MasterIntra::type() const
{
	return type_;
}
