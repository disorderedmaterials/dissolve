/*
	*** Keyword Widget - Bool
	*** src/gui/keywordwidgets/bool_funcs.cpp
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

#include "gui/keywordwidgets/bool.hui"
#include "templates/genericlisthelper.h"

// Constructor
BoolKeywordWidget::BoolKeywordWidget(QWidget* parent, ModuleKeywordBase* keyword, const CoreData& coreData, GenericList& moduleData, const char* prefix) : QCheckBox(parent), KeywordWidgetBase(coreData, moduleData, prefix)
{
	// Cast the pointer up into the parent class type
	keyword_ = dynamic_cast<BoolModuleKeyword*>(keyword);
	if (!keyword_) Messenger::error("Couldn't cast base module keyword '%s' into BoolModuleKeyword.\n", keyword->keyword());
	else
	{
		// Set current value
		setChecked(keyword_->asBool());
	}

	// Connect the
	connect(this, SIGNAL(clicked(bool)), this, SLOT(myClicked(bool)));
}

/*
 * Signals / Slots
 */

// Check box state changed
void BoolKeywordWidget::myClicked(bool checked)
{
	if (refreshing_) return;

	keyword_->setData(checked);

	emit(keywordValueChanged());
}

/*
 * Update
 */

// Update value displayed in widget, using specified source if necessary
void BoolKeywordWidget::updateValue()
{
	refreshing_ = true;

	// Check to see if the associated Keyword may have been stored/updated in the specified moduleData
	if ((keyword_->genericItemFlags()&GenericItem::InRestartFileFlag) && moduleData_.contains(keyword_->keyword(), modulePrefix_))
	{
		// Retrieve the item from the list
		setChecked(GenericListHelper<bool>::value(moduleData_, keyword_->keyword(), modulePrefix_));
	}
	else setChecked(keyword_->asBool());

	refreshing_ = false;
}
