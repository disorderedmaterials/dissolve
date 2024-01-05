// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atomType.h"
#include "classes/atomTypeMix.h"
#include "classes/coreData.h"
#include "gui/keywordWidgets/atomTypeVector.h"
#include "gui/keywordWidgets/dropDown.h"
#include "templates/algorithms.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QString>

AtomTypeVectorKeywordWidget::AtomTypeVectorKeywordWidget(QWidget *parent, AtomTypeVectorKeyword *keyword,
                                                         const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData), keyword_(keyword), atomTypeModel_(coreData)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    // Set up the model
    ui_.AtomTypeList->setModel(&atomTypeModel_);
    atomTypeModel_.setCheckStateData(keyword_->data());
    resetModelData();

    // Connect signals / slots
    connect(&atomTypeModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this,
            SLOT(modelDataChanged(const QModelIndex &, const QModelIndex &)));
}

/*
 * Widgets
 */

void AtomTypeVectorKeywordWidget::modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    if (refreshing_)
        return;

    updateSummaryText();

    emit(keywordDataChanged(keyword_->editSignals()));
}

/*
 * Update
 */

// Reset model data
void AtomTypeVectorKeywordWidget::resetModelData()
{
    refreshing_ = true;

    atomTypeModel_.setData(coreData_.atomTypes());
    atomTypeModel_.setCheckStateData(keyword_->data());

    updateSummaryText();

    refreshing_ = false;
}

// Update value displayed in widget
void AtomTypeVectorKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags) { resetModelData(); }

// Update summary text
void AtomTypeVectorKeywordWidget::updateSummaryText()
{
    if (keyword_->data().empty())
        setSummaryText("<None>");
    else
        setSummaryText(
            QString::fromStdString(joinStrings(keyword_->data(), ", ", [](const auto &at) { return at.get()->name(); })));
}
