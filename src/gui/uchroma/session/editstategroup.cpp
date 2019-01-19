/*
	*** Edit State Group
	*** src/gui/uchroma/session/editstategroup.cpp
	Copyright T. Youngs 2013-2019

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

#include "gui/uchroma/session/editstategroup.h"

// Constructor
EditStateGroup::EditStateGroup()
{
	title_ = "New EditStateGroup";
}

// Destructor
EditStateGroup::~EditStateGroup()
{
}

/*
 * Group Data
 */

// Set title
void EditStateGroup::setTitle(QString title)
{
	title_ = title;
}

// Return title
QString EditStateGroup::title()
{
	return title_;
}

// Add editstate to the list
EditState* EditStateGroup::addEditState(ObjectInfo& target, int quantity)
{
	EditState* state = new EditState(target, quantity);
	editStates_.own(state);
	return state;
}

/*
 * Apply / Revert
 */

// Apply change (old -> new)
bool EditStateGroup::apply(UChromaBase* uChromaBase)
{
	Messenger::printVerbose("EDITSTATEGROUP: applying group '%s'.\n", qPrintable(title_));

	// Loop over edit states
	for (EditState* state = editStates_.first(); state != NULL; state = state->next)
	{
		if (!state->makeChanges(uChromaBase, false)) return false;
	}
	return true;
}

// Revert change (new -> old)
bool EditStateGroup::revert(UChromaBase* uChromaBase)
{
	Messenger::printVerbose("EDITSTATEGROUP: reverting group '%s'.\n", qPrintable(title_));

	// Loop over edit states
	for (EditState* state = editStates_.last(); state != NULL; state = state->prev)
	{
		if (!state->makeChanges(uChromaBase, true)) return false;
	}
	return true;
}
