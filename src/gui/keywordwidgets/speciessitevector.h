// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/ui_speciessitevector.h"
#include "gui/models/sitesFilterProxy.h"
#include "gui/models/sitesModel.h"
#include "keywords/speciessitevector.h"
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

    private slots:
    void modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

    signals:
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
    // Update widget values data based on keyword data
    void updateWidgetValues(const CoreData &coreData) override;
    // Update summary text
    void updateSummaryText();
};
