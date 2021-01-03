// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/ui_addconfigurationdialog.h"
#include <QDialog>

// Forward Declarations
class Dissolve;
class Configuration;

// Add Configuration Dialog
class AddConfigurationDialog : public QDialog
{
    Q_OBJECT

    public:
    AddConfigurationDialog(QWidget *parent, const Dissolve &mainDissolveInstance);
    ~AddConfigurationDialog();

    private:
    // Main form declaration
    Ui::AddConfigurationDialog ui_;

    public:
    // Reset, ready for adding a new Configuration
    void reset();
    // Copy imported Configuration over to the specified Dissolve object
    Configuration *importConfiguration(Dissolve &dissolve);
};
