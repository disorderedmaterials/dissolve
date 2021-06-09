// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/ui_speciessitevector.h"
#include "gui/models/speciesSiteFilterProxy.h"
#include "gui/models/speciesSiteModel.h"
#include "keywords/speciessitevector.h"
#include <QWidget>

// Forward Declarations
class Species;

class SpeciesSiteVectorKeywordWidget : public KeywordDropDown, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    SpeciesSiteVectorKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData);
    ~SpeciesSiteVectorKeywordWidget() override;

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
    Ui::SpeciesSiteRefListWidget ui_;
    // Site models for Species
    std::vector<SpeciesSiteModel *> models_;
    // Proxy filters for sites
    std::vector<SpeciesSiteFilterProxy *> proxyFilters_;

    private slots:
    void modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

    signals:
    // Keyword value changed
    void keywordValueChanged(int flags);

    /*
     * Update
     */
    public:
    // Update value displayed in widget
    void updateValue() override;
    // Update widget values data based on keyword data
    void updateWidgetValues(const CoreData &coreData) override;
    // Update keyword data based on widget values
    void updateKeywordData() override;
    // Update summary text
    void updateSummaryText();
};
