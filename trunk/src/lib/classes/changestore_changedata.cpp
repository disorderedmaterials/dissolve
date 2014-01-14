/*
	*** ChangeStore ChangeData
	*** src/lib/classes/changestore_changedata.cpp
	Copyright T. Youngs 2012-2014

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

#include "classes/changestore.h"
#include "classes/atom.h"

/*!
 * \brief Constructor
 * \details Constructor for ChangeData. 
 */
ChangeData::ChangeData()
{
	atom_ = NULL;
	moved_ = false;
	prev = NULL;
	next = NULL;
}

/*!
 * \brief Destructor
 * \details Destructor for ChangeData. 
 */
ChangeData::~ChangeData()
{
}

/*
// Target Data
*/

/*!
 * \brief Set target atom
 */

void ChangeData::setAtom(Atom* i)
{
#ifdef CHECKS
	if (i == NULL)
	{
		msg.print("NULL_POINTER - NULL Atom pointer passed to ChangeData::setAtom().\n");
		return;
	}
#endif
	atom_ = i;
	moved_ = false;
	r_ = atom_->r();
}

/*!
 * \brief Return index of stored atom
 */
int ChangeData::atomIndex()
{
	return atom_->index();
}

/*!
 * \brief Update local position, and flag as moved
 */
void ChangeData::updatePosition()
{
	r_ = atom_->r();
	moved_ = true;
}

/*!
 * \brief Revert atom to stored position
 */
void ChangeData::revertPosition()
{
	atom_->setCoordinates(r_);
}

/*!
 * \brief Return whether atom has moved
 */
bool ChangeData::hasMoved()
{
	return moved_;
}
