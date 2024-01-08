// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "gui/keywordWidgets/dropDown.h"
#include "gui/keywordWidgets/ui_isotopologueSet.h"
#include "gui/models/isotopologueSetModel.h"
#include "keywords/isotopologueSet.h"
#include <QWidget>

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

    private Q_SLOTS:
    void modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void addSpeciesButton_clicked(bool checked);
    void addIsotopologueButton_clicked(bool checked);
    void removeButton_clicked(bool checked);
    void currentItemChanged();

    Q_SIGNALS:
    // Keyword data changed
    void keywordDataChanged(int flags);

    /*
     * Update
     */
    public:
    // Update value displayed in widget
    void updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags) override;
    // Update summary text
    void updateSummaryText();
};
