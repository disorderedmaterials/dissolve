/*
	*** Keyword Widget - Bool
	*** src/gui/keywordwidgets/bool_funcs.cpp
	Copyright T. Youngs 2012-2017

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

#include "gui/keywordwidgets/bool.hui"
#include "templates/genericlisthelper.h"

// Constructor
KeywordWidgetBool::KeywordWidgetBool(QWidget* parent, ModuleKeywordBase* keyword) : QCheckBox(parent), KeywordWidgetBase()
{
	// Cast the pointer up into the parent class type
	keyword_ = dynamic_cast<BoolModuleKeyword*>(keyword);
	if (!keyword_) Messenger::error("Couldn't cast base module keyword into BoolModuleKeyword.\n");
	else
	{
		// Set current value
		setChecked(keyword_->asBool());
	}

	// Connect the
	connect(this, SIGNAL(clicked(bool)), this, SLOT(myClicked(bool)));
}

/*
 * Slots
 */

// Check box state changed
void KeywordWidgetBool::myClicked(bool checked)
{
	keyword_->setData(checked);
}

/*
 * Update
 */

// Update value displayed in widget, using specified source if necessary
void KeywordWidgetBool::updateValue(GenericList& moduleData, const char* prefix)
{
	// Check to see if the associated Keyword may have been stored/updated in the specified moduleData
	if ((keyword_->genericItemFlags()&GenericItem::InRestartFileFlag) && moduleData.contains(keyword_->keyword(), prefix))
	{
		// Retrieve the item from the list
		setChecked(GenericListHelper<bool>::retrieve(moduleData, keyword_->keyword(), prefix));
	}
	else setChecked(keyword_->asBool());
}
