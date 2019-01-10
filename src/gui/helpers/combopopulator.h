/*
	*** ComboBox Populator
	*** src/gui/helpers/combopopulator.h
	Copyright T. Youngs 2012-2012

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

#ifndef DISSOLVE_COMBOPOPULATOR_H
#define DISSOLVE_COMBOPOPULATOR_H

#include <QComboBox>
#include "templates/list.h"
#include "templates/variantpointer.h"

// ComboBox Populator
class ComboPopulator
{
	public:
	// Constructor
	ComboPopulator(QComboBox* combo, int nItems, const char** itemArray, bool append = false)
	{
		// Clear the combobox
		if (!append) combo->clear();

		// Add our text items to the list
		for (int n=0; n<nItems; ++n) combo->addItem(itemArray[n]);
	}
};

// ComboBox Populator, from templated List using item's name() function
template <class T> class ComboNameListPopulator
{
	public:
	// Constructors
	ComboNameListPopulator<T>(QComboBox* combo, const List<T>& items, bool append = false)
	{
		// Clear the combobox
		if (!append) combo->clear();

		// Add our text items to the list
		for (T* item = items.first(); item != NULL; item = item->next) combo->addItem(item->name(), VariantPointer<T>(item));
	}
	// Constructor
	ComboNameListPopulator<T>(QComboBox* combo, const List<T>& items, const char* prefix, bool append = false)
	{
		// Clear the combobox
		if (!append) combo->clear();

		// Add our text items to the list
		for (T* item = items.first(); item != NULL; item = item->next) combo->addItem(QString("%1%2").arg(prefix, item->name()), VariantPointer<T>(item));
	}
};

#endif
