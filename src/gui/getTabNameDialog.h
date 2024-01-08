// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "gui/ui_getTabNameDialog.h"
#include <QDialog>

// Forward Declarations
class CoreData;
class MainTab;

// Get MainTab Name Dialog
class GetTabNameDialog : public QDialog
{
    Q_OBJECT

    public:
    GetTabNameDialog(QWidget *parent, const std::vector<MainTab *> &currentTabs);
    ~GetTabNameDialog();

    private:
    // Main form declaration
    Ui::GetTabNameDialog ui_;
    // List of all current tabs
    const std::vector<MainTab *> &currentTabs_;
    // Current tab that we are renaming
    const MainTab *currentTab_;

    public:
    // Run the dialog
    bool get(const MainTab *currentTab, const QString &currentName);
    // Return the name string
    QString newName() const;

    /*
     * Slots
     */
    private Q_SLOTS:
    void on_NameEdit_textChanged(const QString text);
    void on_CancelButton_clicked(bool checked);
    void on_OKButton_clicked(bool checked);
};
