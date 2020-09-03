/*
    *** Keyword Widget - Range
    *** src/gui/keywordwidgets/range_funcs.cpp
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
#include "gui/keywordwidgets/range.h"
#include "vec3labels.h"

RangeKeywordWidget::RangeKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData)
{
    // Setup our UI
    ui_.setupUi(this);

    refreshing_ = true;

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<RangeKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into RangeKeyword.\n", keyword->name());
    else
    {
        // Set current values
        ui_.Spin1->setValue(keyword_->data().minimum());
        ui_.Spin2->setValue(keyword_->data().maximum());
    }

    // Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a
    // QScrollArea)
    ui_.Spin1->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.Spin1));
    ui_.Spin2->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.Spin2));

    // Set appropriate labels
    Vec3WidgetLabels::set(ui_.Label1, keyword_->labelType(), 0);
    Vec3WidgetLabels::set(ui_.Label2, keyword_->labelType(), 1);

    refreshing_ = false;
}

/*
 * Widgets
 */

// Spin box value changed
void RangeKeywordWidget::on_Spin1_valueChanged(double value)
{
    if (refreshing_)
        return;

    keyword_->data().setMinimum(value);
    keyword_->hasBeenSet();

    emit(keywordValueChanged(keyword_->optionMask()));
}

// Spin box value changed
void RangeKeywordWidget::on_Spin2_valueChanged(double value)
{
    if (refreshing_)
        return;

    keyword_->data().setMaximum(value);
    keyword_->hasBeenSet();

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void RangeKeywordWidget::updateValue()
{
    refreshing_ = true;

    ui_.Spin1->setValue(keyword_->data().minimum());
    ui_.Spin2->setValue(keyword_->data().maximum());

    refreshing_ = false;
}
