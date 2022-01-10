// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "gui/keywordwidgets/vec3labels.h"
#include "gui/keywordwidgets/vec3nodevalue.h"

Vec3NodeValueKeywordWidget::Vec3NodeValueKeywordWidget(QWidget *parent, Vec3NodeValueKeyword *keyword, const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Setup our UI
    ui_.setupUi(this);

    refreshing_ = true;

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

    keyword_->setData(0, qPrintable(ui_.ValueAEdit->text()));
    ui_.ValueAValidIndicator->setOK(keyword_->data().x.isValid());

    emit(keywordDataChanged(keyword_->editSignals()));
}

void Vec3NodeValueKeywordWidget::on_ValueAEdit_returnPressed()
{
    if (refreshing_)
        return;

    keyword_->setData(0, qPrintable(ui_.ValueAEdit->text()));
    ui_.ValueAValidIndicator->setOK(keyword_->data().x.isValid());

    emit(keywordDataChanged(keyword_->editSignals()));
}

void Vec3NodeValueKeywordWidget::on_ValueBEdit_editingFinished()
{
    if (refreshing_)
        return;

    keyword_->setData(1, qPrintable(ui_.ValueBEdit->text()));
    ui_.ValueBValidIndicator->setOK(keyword_->data().y.isValid());

    emit(keywordDataChanged(keyword_->editSignals()));
}

void Vec3NodeValueKeywordWidget::on_ValueBEdit_returnPressed()
{
    if (refreshing_)
        return;

    keyword_->setData(1, qPrintable(ui_.ValueBEdit->text()));
    ui_.ValueBValidIndicator->setOK(keyword_->data().y.isValid());

    emit(keywordDataChanged(keyword_->editSignals()));
}

void Vec3NodeValueKeywordWidget::on_ValueCEdit_editingFinished()
{
    if (refreshing_)
        return;

    keyword_->setData(2, qPrintable(ui_.ValueCEdit->text()));
    ui_.ValueCValidIndicator->setOK(keyword_->data().z.isValid());

    emit(keywordDataChanged(keyword_->editSignals()));
}

void Vec3NodeValueKeywordWidget::on_ValueCEdit_returnPressed()
{
    if (refreshing_)
        return;

    keyword_->setData(2, qPrintable(ui_.ValueCEdit->text()));
    ui_.ValueCValidIndicator->setOK(keyword_->data().z.isValid());

    emit(keywordDataChanged(keyword_->editSignals()));
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
