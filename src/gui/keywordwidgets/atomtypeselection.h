// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/ui_atomtypeselection.h"
#include "keywords/atomtypeselection.h"
#include <QWidget>
#include <memory>
#include <vector>

// Forward Declarations
class AtomType;

class AtomTypeSelectionKeywordWidget : public KeywordDropDown, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    AtomTypeSelectionKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData);
    // Main form declaration
    Ui::AtomTypeSelectionWidget ui;

    /*
     * Keyword
     */
    private:
    // Associated keyword
    AtomTypeSelectionKeyword *keyword_;
    std::vector<std::shared_ptr<AtomType>> atomTypes_;

    /*
     * Signals / Slots
     */
    private:
    // Selection list update function
    void updateSelectionRow(int row, std::shared_ptr<AtomType> atomType, bool createItem);

    private slots:
    // List item changed
    void itemChanged(QListWidgetItem *item);

    signals:
    // Keyword value changed
    void keywordValueChanged(int flags);

    /*
     * Update
     */
    public:
    // Update value displayed in widget
    void updateValue();
    // Update widget values data based on keyword data
    void updateWidgetValues(const CoreData &coreData);
    // Update keyword data based on widget values
    void updateKeywordData();
    // Update summary text
    void updateSummaryText();
};
