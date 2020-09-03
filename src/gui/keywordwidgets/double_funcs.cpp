/*
    *** Keyword Widget - Double
    *** src/gui/keywordwidgets/double_funcs.cpp
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
#include "gui/keywordwidgets/double.hui"

DoubleKeywordWidget::DoubleKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : ExponentialSpin(parent), KeywordWidgetBase(coreData)
{
    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<DoubleKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into DoubleKeyword.\n", keyword->name());
    else
    {
        // Set minimum and maximum values
        if (keyword_->hasValidationMin())
            setMinimumLimit(keyword_->validationMin());
        if (keyword_->hasValidationMax())
            setMaximumLimit(keyword_->validationMax());

        // Set current value
        setValue(keyword_->asDouble());
    }

    // Connect the valueChanged signal to our own slot
    connect(this, SIGNAL(valueChanged(double)), this, SLOT(myValueChanged(double)));

    // Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a
    // QScrollArea)
    installEventFilter(new MouseWheelWidgetAdjustmentGuard(this));
}

/*
 * Slots
 */

// Spin box value changed
void DoubleKeywordWidget::myValueChanged(double newValue)
{
    if (refreshing_)
        return;

    keyword_->setData(newValue);

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void DoubleKeywordWidget::updateValue()
{
    refreshing_ = true;

    setValue(keyword_->asDouble());

    refreshing_ = false;
}
