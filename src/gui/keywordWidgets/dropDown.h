// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/dropWidget.h"
#include "gui/keywordWidgets/ui_dropDown.h"
#include <QWidget>

// Forward Declarations
class CoreData;

class KeywordDropDown : public QWidget
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    KeywordDropDown(QWidget *parent);

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::KeywordDropDownControlWidget ui_;
    // Widget to display as the drop-down
    DropWidget dropWidget_;

    public:
    // Return the drop widget
    DropWidget *dropWidget();

    /*
     * Signals / Slots
     */
    private Q_SLOTS:
    void on_CallDropWidgetButton_clicked(bool checked);
    void dropWidgetHidden();

    /*
     * Update
     */
    protected:
    // Set summary text on call button
    void setSummaryText(const QString s);
    // Set icon on call button
    void setSummaryIcon(QIcon icon);
    // Update keyword data based on widget values
    virtual void updateKeywordData();
};
