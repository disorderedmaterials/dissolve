/*
	*** Keyword Widget - Integer
	*** src/gui/keywordwidgets/int_funcs.cpp
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

#include "gui/keywordwidgets/int.hui"
#include "templates/genericlisthelper.h"

// Constructor
IntegerKeywordWidget::IntegerKeywordWidget(QWidget* parent, ModuleKeywordBase* keyword) : QSpinBox(parent), KeywordWidgetBase()
{
	// Cast the pointer up into the parent class type
	keyword_ = dynamic_cast<IntegerModuleKeyword*>(keyword);
	if (!keyword_) Messenger::error("Couldn't cast base module keyword into IntegerModuleKeyword.\n");
	else
	{
		// Set minimum and maximum values
		setRange(keyword_->hasValidationMin() ? keyword_->validationMin() : -1e6, keyword_->hasValidationMax() ? keyword_->validationMax() : 1e6);

		// Set current value
		setValue(keyword_->asInt());
	}

	// Connect the
	connect(this, SIGNAL(valueChanged(int)), this, SLOT(myValueChanged(int)));
}


/*
 * Signals / Slots
 */

// Spin box value changed
void IntegerKeywordWidget::myValueChanged(int newValue)
{
	if (refreshing_) return;

	keyword_->setData(newValue);

	emit(keywordValueChanged());
}

/*
 * Update
 */

// Update value displayed in widget, using specified source if necessary
void IntegerKeywordWidget::updateValue(GenericList& moduleData, const char* prefix)
{
	refreshing_ = true;

	// Check to see if the associated Keyword may have been stored/updated in the specified moduleData
	if ((keyword_->genericItemFlags()&GenericItem::InRestartFileFlag) && moduleData.contains(keyword_->keyword(), prefix))
	{
		// Retrieve the item from the list
		setValue(GenericListHelper<int>::retrieve(moduleData, keyword_->keyword(), prefix));
	}
	else setValue(keyword_->asInt());

	refreshing_ = false;
}
