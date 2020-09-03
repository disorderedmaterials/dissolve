/*
    *** Keyword Widget - Vec3Integer
    *** src/gui/keywordwidgets/vec3integer_funcs.cpp
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
#include "gui/keywordwidgets/vec3integer.h"
#include "gui/keywordwidgets/vec3labels.h"

Vec3IntegerKeywordWidget::Vec3IntegerKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData)
{
    // Setup our UI
    ui_.setupUi(this);

    refreshing_ = true;

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<Vec3IntegerKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into Vec3IntegerKeyword.\n", keyword->name());
    else
    {
        // Set minimum and maximum values for each component
        ui_.Spin1->setRange(keyword_->hasValidationMin(0) ? keyword_->validationMin(0) : -1e6,
                            keyword_->hasValidationMax(0) ? keyword_->validationMax(0) : 1e6);
        ui_.Spin2->setRange(keyword_->hasValidationMin(1) ? keyword_->validationMin(1) : -1e6,
                            keyword_->hasValidationMax(1) ? keyword_->validationMax(1) : 1e6);
        ui_.Spin3->setRange(keyword_->hasValidationMin(2) ? keyword_->validationMin(2) : -1e6,
                            keyword_->hasValidationMax(2) ? keyword_->validationMax(2) : 1e6);

        // Set current values
        ui_.Spin1->setValue(keyword_->asVec3Int().x);
        ui_.Spin2->setValue(keyword_->asVec3Int().y);
        ui_.Spin3->setValue(keyword_->asVec3Int().z);
    }

    // Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a
    // QScrollArea)
    ui_.Spin1->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.Spin1));
    ui_.Spin2->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.Spin2));
    ui_.Spin3->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.Spin3));

    // Set appropriate labels
    Vec3WidgetLabels::set(ui_.Label1, keyword_->labelType(), 0);
    Vec3WidgetLabels::set(ui_.Label2, keyword_->labelType(), 1);
    Vec3WidgetLabels::set(ui_.Label3, keyword_->labelType(), 2);

    refreshing_ = false;
}

/*
 * Widgets
 */

// Spin box value changed
void Vec3IntegerKeywordWidget::on_Spin1_valueChanged(int value)
{
    if (refreshing_)
        return;

    auto newVec = keyword_->data();
    newVec.x = value;
    keyword_->setData(newVec);

    emit(keywordValueChanged(keyword_->optionMask()));
}

// Spin box value changed
void Vec3IntegerKeywordWidget::on_Spin2_valueChanged(int value)
{
    if (refreshing_)
        return;

    auto newVec = keyword_->data();
    newVec.y = value;
    keyword_->setData(newVec);

    emit(keywordValueChanged(keyword_->optionMask()));
}

// Spin box value changed
void Vec3IntegerKeywordWidget::on_Spin3_valueChanged(int value)
{
    if (refreshing_)
        return;

    auto newVec = keyword_->data();
    newVec.z = value;
    keyword_->setData(newVec);

    emit(keywordValueChanged(keyword_->optionMask()));
}
/*
 * Update
 */

// Update value displayed in widget
void Vec3IntegerKeywordWidget::updateValue()
{
    refreshing_ = true;

    Vec3<int> v = keyword_->asVec3Int();

    ui_.Spin1->setValue(v.x);
    ui_.Spin2->setValue(v.y);
    ui_.Spin3->setValue(v.z);

    refreshing_ = false;
}
