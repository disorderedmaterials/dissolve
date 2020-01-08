/*
	*** Species Definition - Forcefield
	*** src/classes/species_ff.cpp
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

#include "classes/species.h"
#include "classes/box.h"
#include "data/ff.h"
#include "base/sysfunc.h"

/*
 * Public
 */

// Set Forcefield to source terms from
void Species::setForcefield(Forcefield* ff)
{
	forcefield_ = ff;
}

// Return Forcefield to source terms from
Forcefield* Species::forcefield() const
{
	return forcefield_;
}

// Set whether to auto-generate missing intramolecular terms, and remove invalid ones
void Species::setAutoUpdateIntramolecularTerms(bool b)
{
	autoUpdateIntramolecularTerms_ = b;

	if (autoUpdateIntramolecularTerms_) updateIntramolecularTerms();
}

// Return whether to auto-generate missing intramolecular terms, and remove invalid ones
bool Species::autoUpdateIntramolecularTerms() const
{
	return autoUpdateIntramolecularTerms_;
}

// Apply terms from source Forcefield
bool Species::applyForcefieldTerms(CoreData& coreData)
{
	if (!forcefield_) return Messenger::error("No forcefield set in Species '%s', so can't apply terms.\n", name());

	// Apply the specified Forcefield
	if (!forcefield_->assignAtomTypes(this, coreData, false)) return false;
	if (!forcefield_->assignIntramolecular(this, true, true, true, true, true)) return false;

	return true;
}
