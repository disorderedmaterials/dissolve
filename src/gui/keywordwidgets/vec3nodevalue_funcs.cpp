/*
    *** Keyword Widget - Vec3NodeValue
    *** src/gui/keywordwidgets/vec3nodevalue_funcs.cpp
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

#include "gui/keywordwidgets/vec3labels.h"
#include "gui/keywordwidgets/vec3nodevalue.h"

Vec3NodeValueKeywordWidget::Vec3NodeValueKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData)
{
    // Setup our UI
    ui_.setupUi(this);

    refreshing_ = true;

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<Vec3NodeValueKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into Vec3NodeValueKeyword.\n", keyword->name());
    else
        updateValue();

    // Set appropriate labels
    Vec3WidgetLabels::set(ui_.ValueALabel, keyword_->labelType(), 0);
    Vec3WidgetLabels::set(ui_.ValueBLabel, keyword_->labelType(), 1);
    Vec3WidgetLabels::set(ui_.ValueCLabel, keyword_->labelType(), 2);

    refreshing_ = false;
}

/*
 * Widgets
 */

void Vec3NodeValueKeywordWidget::on_ValueAEdit_editingFinished()
{
    if (refreshing_)
        return;

    keyword_->setValue(0, qPrintable(ui_.ValueAEdit->text()));
    ui_.ValueAValidIndicator->setOK(keyword_->data().x.isValid());

    emit(keywordValueChanged(keyword_->optionMask()));
}

void Vec3NodeValueKeywordWidget::on_ValueAEdit_returnPressed()
{
    if (refreshing_)
        return;

    keyword_->setValue(0, qPrintable(ui_.ValueAEdit->text()));
    ui_.ValueAValidIndicator->setOK(keyword_->data().x.isValid());

    emit(keywordValueChanged(keyword_->optionMask()));
}

void Vec3NodeValueKeywordWidget::on_ValueBEdit_editingFinished()
{
    if (refreshing_)
        return;

    keyword_->setValue(1, qPrintable(ui_.ValueBEdit->text()));
    ui_.ValueBValidIndicator->setOK(keyword_->data().y.isValid());

    emit(keywordValueChanged(keyword_->optionMask()));
}

void Vec3NodeValueKeywordWidget::on_ValueBEdit_returnPressed()
{
    if (refreshing_)
        return;

    keyword_->setValue(1, qPrintable(ui_.ValueBEdit->text()));
    ui_.ValueBValidIndicator->setOK(keyword_->data().y.isValid());

    emit(keywordValueChanged(keyword_->optionMask()));
}

void Vec3NodeValueKeywordWidget::on_ValueCEdit_editingFinished()
{
    if (refreshing_)
        return;

    keyword_->setValue(2, qPrintable(ui_.ValueCEdit->text()));
    ui_.ValueCValidIndicator->setOK(keyword_->data().z.isValid());

    emit(keywordValueChanged(keyword_->optionMask()));
}

void Vec3NodeValueKeywordWidget::on_ValueCEdit_returnPressed()
{
    if (refreshing_)
        return;

    keyword_->setValue(2, qPrintable(ui_.ValueCEdit->text()));
    ui_.ValueCValidIndicator->setOK(keyword_->data().z.isValid());

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void Vec3NodeValueKeywordWidget::updateValue()
{
    refreshing_ = true;

    ui_.ValueAEdit->setText(QString::fromStdString(keyword_->data().x.asString()));
    ui_.ValueAValidIndicator->setOK(keyword_->data().x.isValid());
    ui_.ValueBEdit->setText(QString::fromStdString(keyword_->data().y.asString()));
    ui_.ValueBValidIndicator->setOK(keyword_->data().y.isValid());
    ui_.ValueCEdit->setText(QString::fromStdString(keyword_->data().z.asString()));
    ui_.ValueCValidIndicator->setOK(keyword_->data().z.isValid());

    refreshing_ = false;
}
