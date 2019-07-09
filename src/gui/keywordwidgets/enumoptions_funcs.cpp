/*
	*** Keyword Widget - Enum String
	*** src/gui/keywordwidgets/enumstring_funcs.cpp
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

#include "gui/keywordwidgets/enumstring.hui"
#include "gui/helpers/mousewheeladjustmentguard.h"
#include "genericitems/listhelper.h"

// Constructor
EnumStringKeywordWidget::EnumStringKeywordWidget(QWidget* parent, ModuleKeywordBase* keyword, const CoreData& coreData, GenericList& moduleData, const char* prefix) : QComboBox(parent), KeywordWidgetBase(coreData, moduleData, prefix)
{
	// Cast the pointer up into the parent class type
	keyword_ = dynamic_cast<EnumStringModuleKeyword*>(keyword);
	if (!keyword_) Messenger::error("Couldn't cast base module keyword '%s' into EnumStringModuleKeyword.\n", keyword->keyword());
	else
	{
		// Populate the combo with the allowed values
		const Array<CharString>& items = keyword_->validationList();
		for (int n=0; n<items.nItems(); ++n)
		{
			addItem(items.constAt(n).get());
			if (DissolveSys::sameString(keyword_->asString(), items.constAt(n))) setCurrentIndex(n);
		}

		// Turn off editability
		setEditable(false);
	}

	// Connect the currentTextChanged signal to our own slot
	connect(this, SIGNAL(currentTextChanged(QString)), this, SLOT(myCurrentTextChanged(QString)));

	// Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a QScrollArea)
	installEventFilter(new MouseWheelWidgetAdjustmentGuard(this));
}

/*
 * Signals / Slots
 */

// Combo box text changed
void EnumStringKeywordWidget::myCurrentTextChanged(const QString& text)
{
	if (refreshing_) return;

	keyword_->setData(qPrintable(text));

	emit(keywordValueChanged());
}

/*
 * Update
 */

// Update value displayed in widget, using specified source if necessary
void EnumStringKeywordWidget::updateValue()
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

	// If we have a validation list, set the new index
	if (keyword_->hasValidationList())
	{
		const Array<CharString>& items = keyword_->validationList();
		for (int n=0; n<items.nItems(); ++n) if (DissolveSys::sameString(keyword_->asString(), items.constAt(n)))
		{
			setCurrentIndex(n);
			break;
		}
	}

	refreshing_ = false;
}
