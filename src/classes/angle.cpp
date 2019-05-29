/*
	*** Angle Definition
	*** src/classes/angle.cpp
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

#include "classes/angle.h"
#include "classes/atom.h"
#include "classes/species.h"
#include "base/processpool.h"

// Constructor
Angle::Angle() : Intra(), DynamicArrayObject<Angle>()
{
	clear();
}

// Destructor
Angle::~Angle()
{
}

/*
 * DynamicArrayObject Virtuals
 */

// Clear object, ready for re-use
void Angle::clear()
{
	speciesAngle_ = NULL;
	i_ = NULL;
	j_ = NULL;
	k_ = NULL;

	// Intra
	molecule_ = NULL;
}

/*
 * Atom Information
 */

// Set Atoms involved in Angle
void Angle::setAtoms(Atom* i, Atom* j, Atom* k)
{
	i_ = i;
	j_ = j;
	k_ = k;
#ifdef CHECKS
	if (i_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for Atom* i in Angle::set().\n");
	if (j_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for Atom* j in Angle::set().\n");
	if (k_ == NULL) Messenger::error("NULL_POINTER - NULL pointer passed for Atom* k in Angle::set().\n");
#endif
}

// Return first Atom involved in Angle
Atom *Angle::i() const
{
	return i_;
}

// Return second (central) Atom involved in Angle
Atom *Angle::j() const
{
	return j_;
}

// Return third Atom involved in Angle
Atom *Angle::k() const
{
	return k_;
}

// Return whether Atoms in Angle match those specified
bool Angle::matches(Atom* i, Atom* j, Atom* k) const
{
	if (j_ != j) return false;
	if ((i_ == i) && (k_ == k)) return true;
	if ((i_ == k) && (k_ == i)) return true;
	return false;
}

/*
 * Energy / Force
 */

// Set SpeciesAngle reference
void Angle::setSpeciesAngle(SpeciesAngle* angleRef)
{
	speciesAngle_ = angleRef;
}

// Return SpeciesAngle reference
SpeciesAngle* Angle::speciesAngle() const
{
	return speciesAngle_;
}

// Return energy for specified angle
double Angle::energy(double angleInDegrees) const
{
#ifdef CHECKS
	if (speciesAngle_ == NULL)
	{
		Messenger::warn("Angle:energy() - Angle has no SpeciesAngle set - returning 0.0.\n");
		return 0.0;
	}
#endif
	return speciesAngle_->energy(angleInDegrees);
}

// Return force multiplier for specified angle
double Angle::force(double angleInDegrees) const
{
#ifdef CHECKS
	if (speciesAngle_ == NULL)
	{
		Messenger::warn("Angle:force() - Angle has no SpeciesAngle set - returning 0.0.\n");
		return 0.0;
	}
#endif
	return speciesAngle_->force(angleInDegrees);
}
