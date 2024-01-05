// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/coreData.h"
#include "classes/species.h"
#include "classes/speciesSite.h"
#include "gui/keywordWidgets/speciesSite.h"
#include "templates/variantPointer.h"
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QSpacerItem>

SpeciesSiteKeywordWidget::SpeciesSiteKeywordWidget(QWidget *parent, SpeciesSiteKeyword *keyword, const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    // Connect signals / slots
    connect(ui_.ClearButton, SIGNAL(clicked(bool)), this, SLOT(clearDataButton_clicked(bool)));
    connect(ui_.SpeciesCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateAvailableSites(int)));
    connect(ui_.SiteCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(siteCombo_currentIndexChanged(int)));

    ui_.SpeciesCombo->setModel(&speciesModel_);
    siteFilterProxy_.setSourceModel(&siteModel_);
    ui_.SiteCombo->setModel(&siteFilterProxy_);

    // Set current information
    resetWidgets();
}

/*
 * Widgets
 */

void SpeciesSiteKeywordWidget::clearDataButton_clicked(bool checked)
{
    if (refreshing_)
        return;

    keyword_->data() = nullptr;

    resetWidgets();

    updateSummaryText();

    emit(keywordDataChanged(keyword_->editSignals()));
}

void SpeciesSiteKeywordWidget::siteCombo_currentIndexChanged(int index)
{
    if (refreshing_)
        return;

    // Get the current site, which might be filtered via our proxy.
    auto filteredModelIndex = siteFilterProxy_.index(index, 0);
    if (filteredModelIndex.isValid())
    {
        auto siteIndex = siteFilterProxy_.mapToSource(filteredModelIndex);
        auto *site = siteModel_.data(siteIndex, Qt::UserRole).value<const SpeciesSite *>();
        if (site)
        {
            keyword_->data() = site;

            updateSummaryText();

            emit(keywordDataChanged(keyword_->editSignals()));
        }
    }
}

/*
 * Update
 */

// Update value displayed in widget
void SpeciesSiteKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags)
{
    if (mutationFlags.isSet(DissolveSignals::SpeciesMutated) || mutationFlags.isSet(DissolveSignals::SpeciesSiteMutated))
        resetWidgets();
}

void SpeciesSiteKeywordWidget::updateAvailableSpecies()
{
    refreshing_ = true;

    speciesModel_.setData(coreData_.species());
    if (keyword_->data())
        ui_.SpeciesCombo->setCurrentIndex(std::find_if(coreData_.species().begin(), coreData_.species().end(),
                                                       [&](const auto &sp) { return sp.get() == keyword_->data()->parent(); }) -
                                          coreData_.species().begin());
    else
        ui_.SpeciesCombo->setCurrentIndex(-1);

    refreshing_ = false;
}

void SpeciesSiteKeywordWidget::updateAvailableSites(int speciesIndex)
{
    refreshing_ = true;

    if (speciesIndex == -1)
        siteModel_.setData(std::nullopt);
    else
    {
        auto &sites = coreData_.species()[speciesIndex]->sites();
        siteModel_.setData(sites);
        if (keyword_->axesRequired())
            siteFilterProxy_.setFlag(SpeciesSiteFilterProxy::IsOriented);
        else
            siteFilterProxy_.removeFlag(SpeciesSiteFilterProxy::IsOriented);
        if (keyword_->data())
            ui_.SiteCombo->setCurrentIndex(
                std::find_if(sites.begin(), sites.end(), [&](const auto &site) { return site.get() == keyword_->data(); }) -
                sites.begin());
        else
            ui_.SiteCombo->setCurrentIndex(-1);
    }

    refreshing_ = false;
}

// Reset widgets
void SpeciesSiteKeywordWidget::resetWidgets()
{
    updateAvailableSpecies();
    updateAvailableSites(ui_.SpeciesCombo->currentIndex());

    updateSummaryText();
}

// Update summary text
void SpeciesSiteKeywordWidget::updateSummaryText()
{
    if (keyword_->data())
        setSummaryText(
            QString::fromStdString(fmt::format("{} ({})", keyword_->data()->name(), keyword_->data()->parent()->name())));
    else
        setSummaryText("<None>");
}
