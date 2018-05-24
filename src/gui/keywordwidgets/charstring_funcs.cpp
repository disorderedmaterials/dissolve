/*
	*** Keyword Widget - CharString
	*** src/gui/keywordwidgets/charstring_funcs.cpp
	Copyright T. Youngs 2012-2018

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

#include "gui/keywordwidgets/charstring.hui"
#include "templates/genericlisthelper.h"

// Constructor
CharStringKeywordWidget::CharStringKeywordWidget(QWidget* parent, ModuleKeywordBase* keyword, GenericList& moduleData, const char* prefix) : QComboBox(parent), KeywordWidgetBase(moduleData, prefix)
{
	// Cast the pointer up into the parent class type
	keyword_ = dynamic_cast<CharStringModuleKeyword*>(keyword);
	if (!keyword_) Messenger::error("Couldn't cast base module keyword into CharStringModuleKeyword.\n");
	else
	{
		// Set allowed values
		if (keyword_->hasValidationList())
		{
			// Populate the combo with the allowed values
			const Array<CharString>& items = keyword_->validationList();
			for (int n=0; n<items.nItems(); ++n) addItem(items.value(n).get());

			// Turn off editability
			setEditable(false);

			// Set the current value
			setCurrentIndex(keyword_->indexOfValid(keyword_->asString()));
		}
		else
		{
			setCurrentText(keyword_->asString());
		}
	}

	// Connect the 
	connect(this, SIGNAL(currentTextChanged(QString)), this, SLOT(myCurrentTextChanged(QString)));
}


/*
 * Signals / Slots
 */

// Combo box text changed
void CharStringKeywordWidget::myCurrentTextChanged(const QString& text)
{
	if (refreshing_) return;

	keyword_->setData(qPrintable(text));

	emit(keywordValueChanged());
}

/*
 * Update
 */

// Update value displayed in widget, using specified source if necessary
void CharStringKeywordWidget::updateValue()
{
	refreshing_ = true;

	CharString newValue;
	// Check to see if the associated Keyword may have been stored/updated in the specified moduleData
	if ((keyword_->genericItemFlags()&GenericItem::InRestartFileFlag) && moduleData_.contains(keyword_->keyword(), modulePrefix_))
	{
		// Retrieve the item from the list
		newValue = GenericListHelper<double>::retrieve(moduleData_, keyword_->keyword(), modulePrefix_);
	}
	else newValue = keyword_->asString();

	// Set the new index
	int index = keyword_->indexOfValid(newValue);

	refreshing_ = false;
}

