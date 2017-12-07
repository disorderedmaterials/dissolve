/*
	*** Keyword Widget - CharString
	*** src/gui/keywordwidgets/charstring_funcs.cpp
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

#include "gui/keywordwidgets/charstring.hui"
#include "templates/genericlisthelper.h"

// Constructor
KeywordWidgetCharString::KeywordWidgetCharString(QWidget* parent, ModuleKeywordBase* keyword) : QComboBox(parent), KeywordWidgetBase()
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
 * Slots
 */

// Combo box text changed
void KeywordWidgetCharString::myCurrentTextChanged(const QString& text)
{
	keyword_->setData(qPrintable(text));
}

/*
 * Update
 */

// Update value displayed in widget, using specified source if necessary
void KeywordWidgetCharString::updateValue(GenericList& moduleData, const char* prefix)
{
	CharString newValue;
	// Check to see if the associated Keyword may have been stored/updated in the specified moduleData
	if ((keyword_->genericItemFlags()&GenericItem::InRestartFileFlag) && moduleData.contains(keyword_->keyword(), prefix))
	{
		// Retrieve the item from the list
		newValue = GenericListHelper<double>::retrieve(moduleData, keyword_->keyword(), prefix);
	}
	else newValue = keyword_->asString();

	// Set the new index
	int index = keyword_->indexOfValid(newValue);
}

