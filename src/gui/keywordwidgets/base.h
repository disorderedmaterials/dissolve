/*
	*** Keyword Widget - Base
	*** src/gui/keywordwidgets/base.h
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

#ifndef DISSOLVE_KEYWORDWIDGET_BASE_H
#define DISSOLVE_KEYWORDWIDGET_BASE_H

#include "templates/listitem.h"

// Forward Declarations
class CoreData;
class GenericList;

class KeywordWidgetBase
{
	public:
	// Constructor
	KeywordWidgetBase(const CoreData& coreData);


	/*
	 * Module List
	 */
	protected:
	// CoreData source
	const CoreData& coreData_;


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
