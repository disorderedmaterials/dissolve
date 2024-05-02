// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/ui_getConfigurationNameDialog.h"
#include <QDialog>

// Forward Declarations
class CoreData;
class Configuration;

// Get Configuration Name Dialog
class GetConfigurationNameDialog : public QDialog
{
    Q_OBJECT

    public:
    GetConfigurationNameDialog(QWidget *parent, const CoreData &coreData);
    ~GetConfigurationNameDialog();

    private:
    // Main form declaration
    Ui::GetConfigurationNameDialog ui_;
    // CoreData reference
    const CoreData &coreData_;
    // Current Configuration that we are renaming
    const Configuration *configuration_;

    public:
    // Run the dialog
    bool get(const Configuration *currentConfiguration, const QString &currentName);
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
