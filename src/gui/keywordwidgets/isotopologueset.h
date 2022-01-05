// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/ui_isotopologueset.h"
#include "gui/models/isotopologueSetModel.h"
#include "keywords/isotopologueset.h"
#include <QWidget>

// Forward Declarations
class QComboBox;

class IsotopologueSetKeywordWidget : public KeywordDropDown, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    IsotopologueSetKeywordWidget(QWidget *parent, IsotopologueSetKeyword *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    IsotopologueSetKeyword *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::IsotopologueSetWidget ui_;
    // Model for treeview
    IsotopologueSetModel setModel_;

    private:
    // Return valid Isotopologue names for specified model index
    std::vector<std::string> availableIsotopologueNames(const QModelIndex &index);

    private slots:
    void modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void addSpeciesButton_clicked(bool checked);
    void addIsotopologueButton_clicked(bool checked);
    void removeButton_clicked(bool checked);
    void currentItemChanged();

    signals:
    // Keyword data changed
    void keywordDataChanged(int flags);

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
