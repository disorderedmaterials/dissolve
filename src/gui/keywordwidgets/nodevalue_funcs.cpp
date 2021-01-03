// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/keywordwidgets/nodevalue.h"

NodeValueKeywordWidget::NodeValueKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData)
{
    // Setup our UI
    ui_.setupUi(this);

    refreshing_ = true;

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<NodeValueKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into NodeValueKeyword.\n", keyword->name());
    else
    {
        // Set expression text
        ui_.ValueEdit->setText(QString::fromStdString(keyword_->data().asString()));
        checkValueValidity();
    }

    refreshing_ = false;
}

/*
 * Widgets
 */

void NodeValueKeywordWidget::on_ValueEdit_editingFinished()
{
    if (refreshing_)
        return;

    keyword_->setValue(qPrintable(ui_.ValueEdit->text()));
    checkValueValidity();

    emit(keywordValueChanged(keyword_->optionMask()));
}

void NodeValueKeywordWidget::on_ValueEdit_returnPressed()
{
    if (refreshing_)
        return;

    keyword_->setValue(qPrintable(ui_.ValueEdit->text()));
    checkValueValidity();

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Check validity of current value
void NodeValueKeywordWidget::checkValueValidity() { ui_.ValueValidIndicator->setOK(keyword_->data().isValid()); }

// Update value displayed in widget
void NodeValueKeywordWidget::updateValue()
{
    refreshing_ = true;

    ui_.ValueEdit->setText(QString::fromStdString(keyword_->data().asString()));

    refreshing_ = false;
}
