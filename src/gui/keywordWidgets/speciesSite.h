// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "gui/keywordWidgets/dropDown.h"
#include "gui/keywordWidgets/ui_speciesSite.h"
#include "gui/models/speciesModel.h"
#include "gui/models/speciesSiteFilterProxy.h"
#include "gui/models/speciesSiteModel.h"
#include "keywords/speciesSite.h"
#include <QWidget>

// Forward Declarations
class Species;

class SpeciesSiteKeywordWidget : public KeywordDropDown, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    SpeciesSiteKeywordWidget(QWidget *parent, SpeciesSiteKeyword *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    SpeciesSiteKeyword *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::SpeciesSiteWidget ui_;
    // Model for species
    SpeciesModel speciesModel_;
    // Sites model and filter proxy
    SpeciesSiteModel siteModel_;
    SpeciesSiteFilterProxy siteFilterProxy_;

    private Q_SLOTS:
    void clearDataButton_clicked(bool checked);
    void siteCombo_currentIndexChanged(int index);

    Q_SIGNALS:
    // Keyword data changed
    void keywordDataChanged(int flags);

    /*
     * Update
     */
    private Q_SLOTS:
    // Update combo boxes
    void updateAvailableSpecies();
    void updateAvailableSites(int speciesIndex = -1);

    private:
    // Reset widgets
    void resetWidgets();

    public:
    // Update value displayed in widget
    void updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags) override;
    // Update summary text
    void updateSummaryText();
};
