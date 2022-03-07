// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/coredata.h"
#include "classes/species.h"
#include "classes/speciessite.h"
#include "gui/keywordwidgets/speciessitevector.h"
#include "templates/algorithms.h"

SpeciesSiteVectorKeywordWidget::SpeciesSiteVectorKeywordWidget(QWidget *parent, SpeciesSiteVectorKeyword *keyword,
                                                               const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    sitesFilterProxy_.setSourceModel(&sites_);
    if (keyword->axesRequired())
        sitesFilterProxy_.setFlag(SitesFilterProxy::HasAxes);
    ui_.SitesTree->setModel(&sitesFilterProxy_);
    sites_.setCheckStateData(keyword_->data());
    connect(&sites_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)), this,
            SLOT(modelDataChanged(const QModelIndex &, const QModelIndex &)));

    // Summary text on KeywordDropDown button
    setSummaryText("<None>");
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

// Update value displayed in widget
void SpeciesSiteVectorKeywordWidget::updateValue() { updateWidgetValues(coreData_); }

// Update widget values data based on keyword data
void SpeciesSiteVectorKeywordWidget::updateWidgetValues(const CoreData &coreData)
{
    refreshing_ = true;

    sites_.setData(coreData.species());
    ui_.SitesTree->expandAll();
    updateSummaryText();

    refreshing_ = false;
}

// Update keyword data based on widget values
void SpeciesSiteVectorKeywordWidget::updateKeywordData()
{
    // Not relevant - Handled via checkbox callbacks
}

// Update summary text
void SpeciesSiteVectorKeywordWidget::updateSummaryText()
{
    QString siteText;
    if (keyword_->data().empty())
        setSummaryText("<None>");
    else
        setSummaryText(QString::fromStdString(joinStrings(keyword_->data(), ", ", [](const auto &site) {
            return fmt::format("{} ({})", site->name(), site->parent()->name());
        })));
}
