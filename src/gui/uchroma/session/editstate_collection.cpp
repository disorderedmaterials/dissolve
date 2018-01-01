/*
	*** EditState - Collection
	*** src/gui/uchroma/session/editstate_collection.cpp
	Copyright T. Youngs 2013-2018

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
#include "gui/uchroma/classes/collection.h"
#include "base/messenger.h"

// Apply change to Collection
bool EditState::changeCollection(const List<EditStateData>& data, bool revert)
{
	// Get Collection Object
	Collection* collectionTarget = Collection::object(targetObject_.id());
	if (!collectionTarget)
	{
		Messenger::print("Internal Error - Collection target with id %i no longer exists.\n", targetObject_.id());
		return false;
	}

	// Variables
	bool result = false;
	int i;
	double d;
	CharString s;

	switch (targetQuantity_)
	{
		case (EditState::CollectionTitleQuantity):
			s = stringData("value", data, result);
			Messenger::printVerbose("EDITSTATE: Collection - set name to '%s'\n", s.get());
			collectionTarget->setName(s);
			break;
		default:
			Messenger::print("Internal Error: Target quantity %i for CollectionObject not handled in changeCollection(QString).\n", targetQuantity_);
			return false;
	}
	return result;
}
