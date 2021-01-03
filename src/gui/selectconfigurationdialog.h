// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/ui_selectconfigurationdialog.h"
#include "templates/list.h"
#include <QDialog>

// Forward Declarations
class Dissolve;
class Configuration;
class SelectConfigurationWidget;

// Select Configuration Dialog
class SelectConfigurationDialog : public QDialog
{
    Q_OBJECT

    public:
    SelectConfigurationDialog(QWidget *parent, const CoreData &coreData, QString dialogTitle);
    ~SelectConfigurationDialog();

    private:
    // Main form declaration
    Ui::SelectConfigurationDialog ui_;

    private slots:
    void on_ConfigurationWidget_speciesSelectionChanged(bool isValid);
    void on_ConfigurationWidget_speciesDoubleClicked();
    void on_SelectButton_clicked(bool checked);
    void on_CancelButton_clicked(bool checked);

    public:
    // Run the dialog, returning a single selected Configuration
    Configuration *selectConfiguration();
    // Run the dialog, returning a list of selected Configuration
    RefList<Configuration> selectConfiguration(int minConfiguration, int maxConfiguration);
};
