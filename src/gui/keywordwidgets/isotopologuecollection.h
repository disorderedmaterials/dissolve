// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "gui/helpers/treewidgetupdater.h"
#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/ui_isotopologuecollection.h"
#include "keywords/isotopologuecollection.h"
#include <QWidget>

// Forward Declarations
class QComboBox;

class IsotopologueCollectionKeywordWidget : public KeywordDropDown, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    IsotopologueCollectionKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    IsotopologueCollectionKeyword *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::IsotopologueRefListWidget ui_;

    private:
    // Return valid Isotopologue names for specified model index
    std::vector<std::string> availableIsotopologueNames(const QModelIndex &index);

    private slots:
    void autoButton_clicked(bool checked);
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
    TreeWidgetItemManager<IsotopologueCollectionKeywordWidget, IsotopologueSet> isotopologueSetsItemManager_;
    TreeWidgetItemManager<IsotopologueCollectionKeywordWidget, Isotopologues> isotopologuesItemManager_;
    TreeWidgetItemManager<IsotopologueCollectionKeywordWidget, IsotopologueWeight> isotopologueWeightItemManager_;

    private:
    // IsotopologueTree root (IsotopologueSet) item update function
    void updateIsotopologueTreeRootItem(QTreeWidgetItem *item, IsotopologueSet &topeSet, bool itemIsNew);
    // IsotopologueTree child (Isotopologues) update function
    void updateIsotopologueTreeChildItem(QTreeWidgetItem *item, Isotopologues &topes, bool itemIsNew);
    // IsotopologueTree sub-child (IsotopologueWeight) update function
    void updateIsotopologueTreeSubChildItem(QTreeWidgetItem *item, IsotopologueWeight &isoWeight, bool itemIsNew);

    public:
    // Update value displayed in widget
    void updateValue();
    // Update widget values data based on keyword data
    void updateWidgetValues(const CoreData &coreData);
    // Update keyword data based on widget values
    void updateKeywordData();
};
