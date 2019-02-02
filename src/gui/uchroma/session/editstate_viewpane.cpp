/*
	*** EditState - ViewPane
	*** src/gui/uchroma/session/editstate_viewpane.cpp
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

#include "gui/uchroma/session/editstate.h"
#include "gui/uchroma/classes/viewpane.h"
#include "base/messenger.h"

// Apply change to ViewPane
bool EditState::changeViewPane(const List<EditStateData>& data, bool revert)
{
	// Get ViewPane Object
	ViewPane* viewPaneTarget = ViewPane::object(targetObject_.id());
	if (!viewPaneTarget)
	{
		Messenger::print("Internal Error - ViewPane target with id %i no longer exists.\n", targetObject_.id());
		return false;
	}

	// Variables
	bool result = false, add;
	int i;
	QString s;
	Collection* collection;

	switch (targetQuantity_)
	{
		// Add/Remove Collection target
		case (EditState::ViewPaneAddCollectionTargetQuantity):
		case (EditState::ViewPaneRemoveCollectionTargetQuantity):
			i = integerData("value", data, result);
			if (!result) return false;
			Messenger::printVerbose("EDITSTATE: ViewPane - %s %s collection target '%i'\n", revert ? "reverting" : "applying", EditState::ViewPaneAddCollectionTargetQuantity ? "add" : "remove", i);
			collection = Collection::object(i);
			if (collection == NULL)
			{
				Messenger::print("Internal Error - Couldn't locate Collection object with id '%i'.\n", i);
				return false;
			}
			add = targetQuantity_ == EditState::ViewPaneAddCollectionTargetQuantity;
			if ( (add && revert) || ((!add) && (!revert)) ) viewPaneTarget->removeCollectionTarget(collection);
			viewPaneTarget->addCollectionTarget(collection);
			break;
		default:
			Messenger::print("Internal Error: Target quantity %i for ViewPane object not handled in EditState::changeViewPane().\n", targetQuantity_);
			return false;
	}
	return result;
}
