// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "gui/keywordWidgets/dropDown.h"
#include "gui/keywordWidgets/ui_speciesSiteVector.h"
#include "gui/models/sitesFilterProxy.h"
#include "gui/models/sitesModel.h"
#include "keywords/speciesSiteVector.h"
#include <QWidget>

// Forward Declarations
class Species;

class SpeciesSiteVectorKeywordWidget : public KeywordDropDown, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    SpeciesSiteVectorKeywordWidget(QWidget *parent, SpeciesSiteVectorKeyword *keyword, const CoreData &coreData);
    ~SpeciesSiteVectorKeywordWidget() override = default;

    /*
     * Keyword
     */
    private:
    // Associated keyword
    SpeciesSiteVectorKeyword *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::SpeciesSiteVectorWidget ui_;
    // Site models for Species
    SitesModel sites_;
    // Proxy filters for sites
    SitesFilterProxy sitesFilterProxy_;

    private Q_SLOTS:
    void modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

    Q_SIGNALS:
    // Keyword data changed
    void keywordDataChanged(int flags);

    /*
     * Update
     */
    private:
    // Reset model data
    void resetModelData();

    public:
    // Update value displayed in widget
    void updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags) override;
    // Update summary text
    void updateSummaryText();
};
