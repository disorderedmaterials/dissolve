/*
	*** Keyword Widget - Base
	*** src/gui/keywordwidgets/base.h
	Copyright T. Youngs 2012-2018

	This file is part of DUQ.

	DUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	DUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with DUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_KEYWORDWIDGETBASE_H
#define DUQ_KEYWORDWIDGETBASE_H

#include "templates/listitem.h"

// Forward Declarations
class GenericList;

class KeywordWidgetBase
{
	public:
	// Constructor
	KeywordWidgetBase(GenericList& moduleData, const char* prefix);


	/*
	 * Module List
	 */
	protected:
	// Module data list to be searched for reading/setting existing widget value
	GenericList& moduleData_;
	// Module prefix to use when searching module data list
	const char* modulePrefix_;


	/*
	 * Update
	 */
	protected:
	// Whether the widget is currently refreshing
	bool refreshing_;

	public:
	// Update value displayed in widget
	virtual void updateValue() = 0;
};

#endif
