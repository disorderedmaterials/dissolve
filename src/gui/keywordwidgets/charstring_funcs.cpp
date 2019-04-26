/*
	*** Keyword Widget - CharString
	*** src/gui/keywordwidgets/charstring_funcs.cpp
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

#include "gui/keywordwidgets/charstring.hui"
#include "templates/genericlisthelper.h"

// Constructor
CharStringKeywordWidget::CharStringKeywordWidget(QWidget* parent, ModuleKeywordBase* keyword, const CoreData& coreData, GenericList& moduleData, const char* prefix) : QLineEdit(parent), KeywordWidgetBase(coreData, moduleData, prefix)
{
	// Cast the pointer up into the parent class type
	keyword_ = dynamic_cast<CharStringModuleKeyword*>(keyword);
	if (!keyword_) Messenger::error("Couldn't cast base module keyword '%s' into CharStringModuleKeyword.\n", keyword->keyword());
	else
	{
		setText(keyword_->asString());
	}

	// Connect the currentTextChanged signal to our own slot
	connect(this, SIGNAL(textChanged(QString)), this, SLOT(myTextChanged(QString)));
}

/*
 * Signals / Slots
 */

// Line edit text changed
void CharStringKeywordWidget::myTextChanged(const QString& text)
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

	// Check to see if the associated Keyword may have been stored/updated in the specified moduleData
	CharString newValue;
	if ((keyword_->genericItemFlags()&GenericItem::InRestartFileFlag) && moduleData_.contains(keyword_->keyword(), modulePrefix_))
	{
		// Retrieve the item from the list
		newValue = GenericListHelper<double>::value(moduleData_, keyword_->keyword(), modulePrefix_);
	}
	else newValue = keyword_->asString();

	refreshing_ = false;
}

