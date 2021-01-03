// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/addconfigurationdialog.h"

AddConfigurationDialog::AddConfigurationDialog(QWidget *parent, const Dissolve &mainDissolveInstance)
{
    ui_.setupUi(this);

    // Set up and connect the AddConfigurationWidget
    ui_.AddConfigurationWidget->setMainDissolveReference(&mainDissolveInstance);
    ui_.AddConfigurationWidget->setCloseButtonAvailable(false);
    connect(ui_.AddConfigurationWidget, SIGNAL(finished()), this, SLOT(accept()));
    connect(ui_.AddConfigurationWidget, SIGNAL(canceled()), this, SLOT(reject()));
}

AddConfigurationDialog::~AddConfigurationDialog() {}

// Reset, ready for adding a new Configuration
void AddConfigurationDialog::reset() { ui_.AddConfigurationWidget->reset(); }

// Copy imported Configuration over to the specified Dissolve object
Configuration *AddConfigurationDialog::importConfiguration(Dissolve &dissolve)
{
    return ui_.AddConfigurationWidget->importConfiguration(dissolve);
}
