// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/dropwidget.hui"
#include "gui/keywordwidgets/ui_dropdown.h"
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
    private slots:
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
    // Update widget values data based on keyword data
    virtual void updateWidgetValues(const CoreData &coreData) = 0;
    // Update keyword data based on widget values
    virtual void updateKeywordData() = 0;
};
