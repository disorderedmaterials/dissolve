/*
	*** Generic List Helper
	*** src/templates/genericlisthelper.h
	Copyright T. Youngs 2012-2018

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

#ifndef DUQ_GENERICLISTHELPER_H
#define DUQ_GENERICLISTHELPER_H

#include "base/genericlist.h"
#include "base/sysfunc.h"
#include "templates/genericitems.h"

// Generic List Helper
template <class T> class GenericListHelper
{
	public:
	// Add new named item of template-guided type to specified list
	static T& add(GenericList& targetList, const char* name, const char* prefix = NULL, int flags = -1)
	{
		// Construct full name
		CharString varName;
		if (DUQSys::isEmpty(prefix)) varName = name;
		else varName.sprintf("%s_%s", prefix, name);

		// Does the named variable already exist in the list?
		GenericItem* existingItem = targetList.find(varName);
		if (existingItem)
		{
			printf("WARNING - Item '%s' already exists in the list - a dummy value will be returned instead.\n", varName.get());
			static T dummy;
			return dummy;
		}

		// Create new item
		GenericItemContainer<T>* newItem = new GenericItemContainer<T>(varName);
		if (flags >= 0) newItem->setFlags(flags);
		targetList.add(newItem);
		return newItem->data;
	}
	// Retrieve named item from specified list as template-guided type
	static T& retrieve(GenericList& sourceList, const char* name, const char* prefix = NULL, T defaultValue = T(), bool* found = NULL)
	{
		// Construct full name
		CharString varName;
		if (DUQSys::isEmpty(prefix)) varName = name;
		else varName.sprintf("%s_%s", prefix, name);

		// Find item in the list
		GenericItem* item = sourceList.find(varName);
		if (!item)
		{
			Messenger::printVerbose("No item named '%s' in list - default value item will be returned.\n", varName.get());
			static T dummy;
			dummy = defaultValue;
			if (found != NULL) (*found) = false;
			return dummy;
		}

		// Cast to correct type
		GenericItemContainer<T>* castItem = (GenericItemContainer<T>*) item;
		if (!castItem)
		{
			printf("That didn't work, because its of the wrong type.\n");
			static T dummy;
			if (found != NULL) (*found) = false;
			return dummy;
		}

		if (found != NULL) (*found) = true;
		return castItem->data;
	}
	// Retrieve (or create and retrieve) named item from specified list as template-guided type
	static T& realise(GenericList& sourceList, const char* name, const char* prefix = NULL, int flags = -1, bool* created = NULL)
	{
		// Construct full name
		CharString varName;
		if (DUQSys::isEmpty(prefix)) varName = name;
		else varName.sprintf("%s_%s", prefix, name);

		// Find item in the list - if it isn't there, create it and return
		GenericItem* item = sourceList.find(varName);
		if (!item)
		{
			if (created != NULL) (*created) = true;
			return add(sourceList, name, prefix, flags);
		}

		// Cast to correct type
		GenericItemContainer<T>* castItem = (GenericItemContainer<T>*) item;
		if (!castItem)
		{
			printf("That didn't work, because its of the wrong type.\n");
			static T dummy;
			return dummy;
		}

		// Update flags
		if (flags >= 0) item->setFlags(flags);

		if (created != NULL) (*created) = false;
		return castItem->data;
	}
};

#endif
