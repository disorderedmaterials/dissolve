// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/keywordWidgets/nodeValue.h"

NodeValueKeywordWidget::NodeValueKeywordWidget(QWidget *parent, NodeValueKeyword *keyword, const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Setup our UI
    ui_.setupUi(this);

    refreshing_ = true;

    // Set expression text
    ui_.ValueEdit->setText(QString::fromStdString(keyword_->data().asString()));
    checkValueValidity();

    refreshing_ = false;
}

/*
 * Widgets
 */

void NodeValueKeywordWidget::on_ValueEdit_editingFinished()
{
    if (refreshing_)
        return;

    keyword_->setData(qPrintable(ui_.ValueEdit->text()));
    checkValueValidity();

    emit(keywordDataChanged(keyword_->editSignals()));
}

void NodeValueKeywordWidget::on_ValueEdit_returnPressed()
{
    if (refreshing_)
        return;

    keyword_->setData(qPrintable(ui_.ValueEdit->text()));
    checkValueValidity();

    emit(keywordDataChanged(keyword_->editSignals()));
}

/*
 * Update
 */

// Check validity of current value
void NodeValueKeywordWidget::checkValueValidity() { ui_.ValueValidIndicator->setOK(keyword_->data().isValid()); }

// Update value displayed in widget
void NodeValueKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags)
{
    refreshing_ = true;

    ui_.ValueEdit->setText(QString::fromStdString(keyword_->data().asString()));

    refreshing_ = false;
}
