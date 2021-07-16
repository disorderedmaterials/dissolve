// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/coredata.h"
#include "classes/species.h"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/keywordwidgets/speciesvector.h"
#include "templates/algorithms.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QString>

SpeciesVectorKeywordWidget::SpeciesVectorKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());
    ui_.SpeciesList->setModel(&speciesModel_);

    // Connect signals / slots
    connect(&speciesModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this,
            SLOT(modelDataChanged(const QModelIndex &, const QModelIndex &)));

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<SpeciesVectorKeyword *>(keyword);
    if (!keyword_)
        throw(std::runtime_error(fmt::format("Couldn't cast base keyword '{}' into SpeciesVectorKeyword.\n", keyword->name())));
    speciesModel_.setCheckStateData(keyword_->data());
}

/*
 * Widgets
 */

void SpeciesVectorKeywordWidget::modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    if (refreshing_)
        return;

    keyword_->setAsModified();

    updateSummaryText();

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void SpeciesVectorKeywordWidget::updateValue() { updateWidgetValues(coreData_); }

// Update widget values data based on keyword data
void SpeciesVectorKeywordWidget::updateWidgetValues(const CoreData &coreData)
{
    refreshing_ = true;

    speciesModel_.setData(coreData.species());
    speciesModel_.setCheckStateData(keyword_->data());

    updateSummaryText();

    refreshing_ = false;
}

// Update keyword data based on widget values
void SpeciesVectorKeywordWidget::updateKeywordData()
{
    // Handled by model
}

// Update summary text
void SpeciesVectorKeywordWidget::updateSummaryText()
{
    if (keyword_->data().empty())
        setSummaryText("<None>");
    else
        setSummaryText(QString::fromStdString(joinStrings(keyword_->data(), ", ", [](const auto &sp) { return sp->name(); })));
}
