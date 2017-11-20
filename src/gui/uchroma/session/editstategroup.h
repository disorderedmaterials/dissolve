/*
	*** Edit State Group
	*** src/gui/uchroma/session/editstategroup.h
	Copyright T. Youngs 2013-2017

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_UCHROMAEDITSTATEGROUP_H
#define DUQ_UCHROMAEDITSTATEGROUP_H

#include <QString>
#include "gui/uchroma/session/editstate.h"
#include "gui/uchroma/session/editstatedata.h"

// Forward Declarations
/* none */

// Edit State Group
class EditStateGroup : public ListItem<EditStateGroup>
{
	public:
	// Constructor / destructor
	EditStateGroup();
	~EditStateGroup();


	/*
	 * State Data
	 */
	private:
	// Title
	QString title_;
	// List of edit states
	List<EditState> editStates_;

	public:
	// Set title
	void setTitle(QString title);
	// Return title
	QString title();
	// Add edit state to group
	EditState* addEditState(ObjectInfo& target, int quantity);


	/*
	 * Apply / Revert
	 */
	public:
	// Apply change (old -> new)
	bool apply();
	// Revert change (new -> old)
	bool revert();
};

#endif
