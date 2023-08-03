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
    : KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(this);

    // Set up the model
    ui_.RangeVectorTable->setModel(&rangeModel_);
    ui_.RangeVectorTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    resetModelData();

    // Set delegates
    ui_.RangeVectorTable->setItemDelegateForColumn(1, new ExponentialSpinDelegate(this));

    // Connect signals / slots
    connect(&rangeModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this,
            SLOT(modelDataChanged(const QModelIndex &, const QModelIndex &)));
}

/*
 * Widgets
 */

// List item changed
void RangeVectorKeywordWidget::modelDataChanged(const QModelIndex &, const QModelIndex &)
{
    if (refreshing_)
        return;

    emit(keywordDataChanged(keyword_->editSignals()));
}

void RangeVectorKeywordWidget::on_RangeAddButton_clicked(bool checked)
{
    rangeModel_.insertRows(rangeModel_.rowCount(), 1);
    // rangeModel_.addRange();
}

void RangeVectorKeywordWidget::on_RangeRemoveButton_clicked(bool checked) { rangeModel_.removeRows(selectedRow(), 1); }

/*
 * Update
 */

// Reset model data
void RangeVectorKeywordWidget::resetModelData()
{
    refreshing_ = true;

    // Update allowed modules
    rangeModel_.setData(keyword_->data());

    refreshing_ = false;
}

// Update value displayed in widget
void RangeVectorKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags)
{
    if (mutationFlags.isSet(DissolveSignals::ModulesMutated))
        resetModelData();
}

int RangeVectorKeywordWidget::selectedRow() const { return ui_.RangeVectorTable->selectionModel()->currentIndex().row(); }