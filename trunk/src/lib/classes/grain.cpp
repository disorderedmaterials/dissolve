/*
	*** Grain
	*** src/lib/classes/grain.cpp
	Copyright T. Youngs 2012-2013

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

#include "classes/grain.h"
#include "classes/graindefinition.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/potentialmap.h"
#include "classes/angle.h"

/*!
 * \brief Constructor
 * \details Constructor for Grain. 
 */
Grain::Grain()
{
	grainDefinition_ = NULL;
	atoms_ = NULL;
	nAtoms_ = 0;
	localIndex_ = -1;
	cell_ = NULL;
	index_ = -1;
	parent_ = NULL;
}

/*!
 * \brief Destructor
 * \details Destructor for Grain. 
 */
Grain::~Grain()
{
	clear();
}

/*!
 * \brief Clear all data
 */
void Grain::clear()
{
	if (atoms_ != NULL) delete[] atoms_;
	atoms_ = NULL;
	nAtoms_ = 0;
	atomsArraySize_ = 0;
	centre_.zero();
}

/*
// Atoms
*/

/*!
 * \brief Initialise
 * \details Initialise the Grain structure based on the GrainDefinition supplied. The resulting Grain will have enough
 * storage to reference the number of Atoms contained in the original GrainDefinition.
 */
bool Grain::initialise(GrainDefinition* gd)
{
	clear();
#ifdef CHECKS
	// Check for NULL pointer
	if (gd == NULL)
	{
		msg.error("NULL_POINTER - NULL GrainDefinition pointer passed to Grain::initialise().\n");
		return FALSE;
	}
#endif
	grainDefinition_ = gd;
	if (grainDefinition_->nAtoms() < 1)
	{
		msg.print("BAD_ARRAY - Tried to initialise an array of size %i in Grain::initialise().\n", grainDefinition_->nAtoms());
		return FALSE;
	}
	atomsArraySize_ = grainDefinition_->nAtoms();
	atoms_ = new Atom*[atomsArraySize_];
	nAtoms_ = 0;
	return TRUE;
}

/*!
 * \brief Initialise
 * \details Initialise the Grain structure to hold at most maxAtoms Atoms.
 */
bool Grain::initialise(int maxAtoms, int newIndex)
{
	clear();
	grainDefinition_ = NULL;
	if (maxAtoms < 1)
	{
		msg.print("BAD_ARRAY - Tried to initialise an array of size %i in Grain::initialise().\n", maxAtoms);
		return FALSE;
	}
	atomsArraySize_ = maxAtoms;
	atoms_ = new Atom*[atomsArraySize_];
	nAtoms_ = 0;
	index_ = newIndex;
	centre_.zero();
	return TRUE;
}

/*!
 * \brief Set parent Molecule
 */
void Grain::setParent(Molecule* mol)
{
	parent_ = mol;
}

/*!
 * \brief Return parent Molecule
 */
Molecule* Grain::parent() const
{
	return parent_;
}

/*!
 * \brief Add Atom pointer to list
 */
bool Grain::addAtom(Atom* i)
{
#ifdef CHECKS
	// Check for NULL pointer
	if (i == NULL)
	{
		msg.error("NULL_POINTER - NULL Atom pointer passed to Grain::addAtom().\n");
		return FALSE;
	}
#endif
	if (nAtoms_ < atomsArraySize_) 
	{
		atoms_[nAtoms_] = i;
		i->setGrain(this);

		// Update centre and increase nAtoms_
		centre_ *= nAtoms_;
		centre_ += i->r();
		++nAtoms_;
		centre_ /= nAtoms_;
	}
	else
	{
		msg.error("Tried to add too many Atoms to Grain.\n");
		return FALSE;
	}
	return TRUE;
}

/*!
 * \brief Return number of Atoms in Grain
 */
int Grain::nAtoms() const
{
	return nAtoms_;
}

/*!
 * \brief Return nth Atom in Grain
 */
Atom* Grain::atom(int n) const
{
#ifdef CHECKS
	if ((n < 0) || (n >= nAtoms_))
	{
		msg.print("OUT_OF_RANGE - Atom index (%i) given to Grain::atom() is out of range (nAtoms_ = %i).\n", n, nAtoms_);
		return NULL;
	}
#endif
	return atoms_[n];
}

/*!
 * \brief Set index of Grain
 */
void Grain::setIndex(int index)
{
#ifdef CHECKS
	if (index_ != -1)
	{
		msg.print("BAD_USAGE - Tried to set the index of a Grain twice.\n");
		return;
	}
#endif
	index_ = index;
}

/*!
 * \brief Return index of Grain
 */
int Grain::index() const
{
	return index_;
}

/*!
 * \brief Return name of Grain
 * \details Return the name of the original GrainDefinition on which this Grain is based.
 */
const char* Grain::name() const
{
#ifdef CHECKS
	if (grainDefinition_ == NULL)
	{
		msg.error("NULL_POINTER - Requested name of Grain, but GrainDefinition pointer is NULL.\n");
		return "???";
	}
#endif
	return grainDefinition_->name();
}

/*
 * Connections
 */

/*!
 * \brief Clear all intra- and inter-Grain terms
 */
void Grain::clearIntramolecular()
{
	internalBonds_.clear();
	internalAngles_.clear();
	bondConnections_.clear();
	angleConnections_.clear();
}

/*!
 * \brief Add intra-Grain Bond
 */
void Grain::addInternalBond(Bond* b)
{
	internalBonds_.add(b);
}

/*!
 * \brief Return first local intra-Grain Bond
 */
RefListItem<Bond,int>* Grain::internalBonds() const
{
	return internalBonds_.first();
}

/*!
 * \brief Add intra-Grain Angle
 */
void Grain::addInternalAngle(Angle* a)
{
	internalAngles_.add(a);
}

/*!
 * \brief Return first local intra-Grain Angle
 */
RefListItem< Angle, int >* Grain::internalAngles() const
{
	return internalAngles_.first();
}

/*!
 * \brief Add Bond connection
 */
void Grain::addBondConnection(Bond* b)
{
	bondConnections_.add(b);
}

/*!
 * \brief Return first Bond connection
 */
RefListItem<Bond,int>* Grain::bondConnections() const
{
	return bondConnections_.first();
}

/*!
 * \brief Add Angle connection
 */
void Grain::addAngleConnection(Angle* a)
{
	angleConnections_.add(a);
}

/*!
 * \brief Return first Angle connection
 */
RefListItem<Angle,int>* Grain::angleConnections() const
{
	return angleConnections_.first();
}

/*
// Coordinates / Manipulation
*/

/*!
 * \brief Adjust centre
 * \details Update the centre of geometry of the Grain, following the shift of a single Atom by amount given
 */
void Grain::updateCentre(const Vec3<double>& atomDeltaR)
{
	// Multiply old coordinates by nAtoms_ and add deltaR
	Vec3<double> newCentre = centre_*nAtoms_ + atomDeltaR;
	centre_ = newCentre / nAtoms_;
}

/*!
 * \brief Return centre of geometry of Grain
 */
const Vec3<double>& Grain::centre() const
{
	return centre_;
}

/*!
 * \brief Set Cell location
 */
void Grain::setCell(Cell* cell, int index)
{
#ifdef CHECKS
	if (cell == NULL)
	{
		msg.print("BAD_USAGE - Don't pass a NULL cell to Grain::setCell() - call Grain::removeFromCell() instead.\n");
		return;
	}
	// Check for a different Cell assignment (ok to pass same cell, since we allow index to be updated)
	if ((cell_ != NULL) && (cell_ != cell))
	{
		msg.print("BAD_USAGE - Refused to set Cell within Grain '%s' since it is still associated to a Cell.\n", name());
		return;
	}
#endif
	cell_ = cell;
	localIndex_ = index;
}

/*!
 * \brief Remove Grain from its current Cell
 */
void Grain::removeFromCell(Cell* caller)
{
#ifdef CHECKS
	if (cell_ == NULL)
	{
		msg.error("NULL_POINTER - Tried to remove Grain '%s' from its Cell, but Cell is NULL.\n", name());
		return;
	}
#endif
	if (!caller) cell_->removeGrain(this);
	cell_ = NULL;
	localIndex_ = -1;
}

/*!
 * \brief Return Cell location
 */
Cell* Grain::cell() const
{
	return cell_;
}

/*!
 * \brief Return local index of Grain in Cell's list
 */
int Grain::cellListIndex() const
{
	return localIndex_;
}

/*!
 * \brief Move Grain centre
 */
void Grain::moveTo(const Vec3<double>& target)
{
	Vec3<double> delta = target - centre_;
	for (int n=0; n<nAtoms_; ++n) atoms_[n]->translateCoordinatesNasty(delta);
	centre_ = target;
}

/*!
 * \brief Translate Grain centre
 */
void Grain::translate(const Vec3<double>& delta)
{
	for (int n=0; n<nAtoms_; ++n) atoms_[n]->translateCoordinatesNasty(delta);
	centre_ += delta;
}
