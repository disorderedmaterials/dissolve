// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/coreData.h"
#include "gui/delegates/exponentialSpin.hui"
#include "gui/keywordWidgets/dropDown.h"
#include "gui/keywordWidgets/rangeVector.h"
#include "templates/algorithms.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QString>

RangeVectorKeywordWidget::RangeVectorKeywordWidget(QWidget *parent, RangeVectorKeyword *keyword, const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    // Set up the model
    ui_.RangeVectorTable->setModel(&rangeModel_);
    resetModelData();

    // Set delegates
    ui_.RangeVectorTable->setItemDelegateForColumn(1, new ExponentialSpinDelegate(this));

    // Connect signals / slots
    connect(&rangeModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this,
            SLOT(modelDataChanged(const QModelIndex &, const QModelIndex &)));

    updateSummaryText();
}

/*
 * Widgets
 */

// List item changed
void RangeVectorKeywordWidget::modelDataChanged(const QModelIndex &, const QModelIndex &)
{
    if (refreshing_)
        return;

    updateSummaryText();

    emit(keywordDataChanged(keyword_->editSignals()));
}

void RangeVectorKeywordWidget::on_RangeAddButton_clicked(bool checked) { rangeModel_.addRange(); }

void RangeVectorKeywordWidget::on_RangeRemoveButton_clicked(bool checked) {}

/*
 * Update
 */

// Reset model data
void RangeVectorKeywordWidget::resetModelData()
{
    refreshing_ = true;

    // Update allowed modules
    rangeModel_.setData(keyword_->data());

    updateSummaryText();

    refreshing_ = false;
}

// Update value displayed in widget
void RangeVectorKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags)
{
    if (mutationFlags.isSet(DissolveSignals::ModulesMutated))
        resetModelData();
}

// Update summary text
void RangeVectorKeywordWidget::updateSummaryText()
{
    setSummaryText(QString::fromStdString(fmt::format("{} defined", keyword_->data().size())));
}