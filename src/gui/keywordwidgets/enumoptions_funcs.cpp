/*
    *** Keyword Widget - EnumOptions
    *** src/gui/keywordwidgets/enumoptions_funcs.cpp
    Copyright T. Youngs 2012-2020

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

#include "genericitems/listhelper.h"
#include "gui/helpers/mousewheeladjustmentguard.h"
#include "gui/keywordwidgets/enumoptions.hui"

EnumOptionsKeywordWidget::EnumOptionsKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : QComboBox(parent), KeywordWidgetBase(coreData)
{
    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<EnumOptionsBaseKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into EnumOptionsBaseKeyword.\n", keyword->name());
    else
    {
        // Get the underlying EnumOptionsBase
        const EnumOptionsBase &options = keyword_->baseOptions();

        // Populate the combo with the available keywords
        for (int n = 0; n < options.nOptions(); ++n)
        {
            addItem(QString::fromStdString(std::string(options.keywordByIndex(n))));
            if (options.currentOptionIndex() == n)
                setCurrentIndex(n);
        }

        // Turn off editability
        setEditable(false);
    }

    // Connect the currentTextChanged signal to our own slot
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(myCurrentIndexChanged(int)));

    // Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a
    // QScrollArea)
    installEventFilter(new MouseWheelWidgetAdjustmentGuard(this));
}

/*
 * Signals / Slots
 */

// Combo box index changed
void EnumOptionsKeywordWidget::myCurrentIndexChanged(int index)
{
    if (refreshing_)
        return;

    // Use the virtual EnumOptionsBaseKeyword::setEnumerationByIndex() to set the new option and inform the underlying
    // keyword structure that it has been modified
    keyword_->setEnumerationByIndex(index);

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void EnumOptionsKeywordWidget::updateValue()
{
    refreshing_ = true;

    // Set the combo box index
    setCurrentIndex(keyword_->baseOptions().currentOptionIndex());

    refreshing_ = false;
}
