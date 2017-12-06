/*
	*** Keyword Widget - Double
	*** src/gui/keywordwidgets/double_funcs.cpp
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

#include "gui/keywordwidgets/double.hui"
#include "templates/genericlist.h"

// Constructor
KeywordWidgetDouble::KeywordWidgetDouble(QWidget* parent, ModuleKeywordBase* keyword) : TExponentialSpin(parent), KeywordWidgetBase()
{
	// Cast the pointer up into the parent class type
	keyword_ = dynamic_cast<DoubleModuleKeyword*>(keyword);
	if (!keyword_) Messenger::error("Couldn't cast base module keyword into DoubleModuleKeyword.\n");
	else
	{
		// Set minimum and maximum values
		setRange(keyword_->hasValidationMin(), keyword_->validationMin(), keyword_->hasValidationMax(), keyword_->validationMax());

		// Set current value
		setValue(keyword_->asDouble());
	}

	// Connect the valueChanged signal to our own slot
	connect(this, SIGNAL(valueChanged(double)), this, SLOT(myValueChanged(double)));
}


/*
 * Slots
 */

// Spin box value changed
void KeywordWidgetDouble::myValueChanged(double newValue)
{
	keyword_->setData(newValue);
}

/*
 * Update
 */

// Update value displayed in widget, using specified source if necessary
void KeywordWidgetDouble::updateValue(GenericList& moduleData, const char* prefix)
{
	// Check to see if the associated Keyword may have been stored/updated in the specified moduleData
	if ((keyword_->genericItemFlags()&GenericItem::InRestartFileFlag) && moduleData.contains(keyword_->keyword(), prefix))
	{
		// Retrieve the item from the list
		setValue(GenericListHelper<double>::retrieve(moduleData, keyword_->keyword(), prefix));
	}
	else setValue(keyword_->asDouble());
}
