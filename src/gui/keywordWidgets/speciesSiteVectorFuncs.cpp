// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/coreData.h"
#include "classes/species.h"
#include "classes/speciesSite.h"
#include "gui/keywordWidgets/speciesSiteVector.h"
#include "templates/algorithms.h"

SpeciesSiteVectorKeywordWidget::SpeciesSiteVectorKeywordWidget(QWidget *parent, SpeciesSiteVectorKeyword *keyword,
                                                               const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    // Set up the model
    sitesFilterProxy_.setSourceModel(&sites_);
    if (keyword->axesRequired())
        sitesFilterProxy_.setFlag(SitesFilterProxy::HasAxes);
    ui_.SitesTree->setModel(&sitesFilterProxy_);
    sites_.setCheckStateData(keyword_->data());
    resetModelData();

    // Connect signals / slots
    connect(&sites_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)), this,
            SLOT(modelDataChanged(const QModelIndex &, const QModelIndex &)));
}

/*
 * Widgets
 */

void SpeciesSiteVectorKeywordWidget::modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
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
void SpeciesSiteVectorKeywordWidget::resetModelData()
{
    refreshing_ = true;

    sites_.setData(coreData_.species());
    ui_.SitesTree->expandAll();
    updateSummaryText();

    refreshing_ = false;
}

// Update value displayed in widget
void SpeciesSiteVectorKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags)
{
    if (mutationFlags.isSet(DissolveSignals::SpeciesMutated) || mutationFlags.isSet(DissolveSignals::SpeciesSiteMutated))
        resetModelData();
}

// Update summary text
void SpeciesSiteVectorKeywordWidget::updateSummaryText()
{
    QString siteText;
    if (keyword_->data().empty())
        setSummaryText("<None>");
    else
        setSummaryText(QString::fromStdString(
            joinStrings(keyword_->data(), ", ",
                        [](const auto &site) { return fmt::format("{} ({})", site->name(), site->parent()->name()); })));
}
