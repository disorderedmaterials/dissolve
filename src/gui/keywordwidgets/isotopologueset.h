// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/helpers/treewidgetupdater.h"
#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/ui_isotopologueset.h"
#include "keywords/isotopologueset.h"
#include <QWidget>

// Forward Declarations
class QComboBox;

class IsotopologueSetKeywordWidget : public KeywordDropDown, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    IsotopologueSetKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData);

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

    private:
    // Return valid Isotopologue names for specified model index
    std::vector<std::string> availableIsotopologueNames(const QModelIndex &index);

    private slots:
    void addButton_clicked(bool checked);
    void removeButton_clicked(bool checked);
    void isotopologueTree_itemChanged(QTreeWidgetItem *item, int column);
    void isotopologueTree_currentItemChanged(QTreeWidgetItem *currentItem, QTreeWidgetItem *previousItem);

    signals:
    // Keyword value changed
    void keywordValueChanged(int flags);

    /*
     * Update
     */
    private:
    // Tree widget item managers
    TreeWidgetItemManager<IsotopologueSetKeywordWidget, Isotopologues> isotopologuesItemManager_;
    TreeWidgetItemManager<IsotopologueSetKeywordWidget, IsotopologueWeight> isotopologueWeightItemManager_;

    private:
    // IsotopologueTree root (Isotopologues) item update function
    void updateIsotopologueTreeRootItem(QTreeWidgetItem *item, Isotopologues &topes, bool itemIsNew);
    // IsotopologueTree child (IsotopologueWeight) update function
    void updateIsotopologueTreeChildItem(QTreeWidgetItem *item, IsotopologueWeight &isoWeight, bool itemIsNew);

    public:
    // Update value displayed in widget
    void updateValue();
    // Update widget values data based on keyword data
    void updateWidgetValues(const CoreData &coreData);
    // Update keyword data based on widget values
    void updateKeywordData();
};
