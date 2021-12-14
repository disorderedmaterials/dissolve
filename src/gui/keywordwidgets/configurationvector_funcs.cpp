// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/configuration.h"
#include "classes/coredata.h"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/keywordwidgets/configurationvector.h"
#include "module/module.h"
#include "templates/algorithms.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QString>

ConfigurationVectorKeywordWidget::ConfigurationVectorKeywordWidget(QWidget *parent, ConfigurationVectorKeyword *keyword,
                                                                   const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());
    ui_.ConfigurationList->setModel(&configurationModel_);
    configurationModel_.setCheckStateData(keyword_->data());

    // Connect signals / slots
    connect(&configurationModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this,
            SLOT(modelDataChanged(const QModelIndex &, const QModelIndex &)));
}

/*
 * Widgets
 */

void ConfigurationVectorKeywordWidget::modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    if (refreshing_)
        return;

    updateSummaryText();

    emit(keywordDataChanged(keyword_->signalMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void ConfigurationVectorKeywordWidget::updateValue() { updateWidgetValues(coreData_); }

// Update widget values data based on keyword data
void ConfigurationVectorKeywordWidget::updateWidgetValues(const CoreData &coreData)
{
    refreshing_ = true;

    configurationModel_.setData(coreData.configurations());
    configurationModel_.setCheckStateData(keyword_->data());

    updateSummaryText();

    refreshing_ = false;
}

// Update keyword data based on widget values
void ConfigurationVectorKeywordWidget::updateKeywordData()
{
    // Handled by model
}

// Update summary text
void ConfigurationVectorKeywordWidget::updateSummaryText()
{
    if (keyword_->data().empty())
        setSummaryText("<None>");
    else
        setSummaryText(
            QString::fromStdString(joinStrings(keyword_->data(), ", ", [](const auto &cfg) { return cfg->name(); })));
}
