// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/getgizmonamedialog.h"
// #include "gui/workspacetab.h"
#include "base/sysfunc.h"

GetGizmoNameDialog::GetGizmoNameDialog(QWidget *parent) { ui_.setupUi(this); }

GetGizmoNameDialog::~GetGizmoNameDialog() {}

// Run the dialog
bool GetGizmoNameDialog::get(const Gizmo *currentGizmo, const QString &currentName)
{
    currentGizmo_ = currentGizmo;

    // Set the current text
    ui_.NameEdit->setText(currentName);

    show();

    return (exec() == QDialog::Accepted);
}

// Return the name string
QString GetGizmoNameDialog::newName() const { return ui_.NameEdit->text(); }

/*
 * Slots
 */

// Workspace name edited
void GetGizmoNameDialog::on_NameEdit_textChanged(const QString text)
{
    // Make sure the name is valid
    auto nameValid = true;
    if (text.isEmpty())
        nameValid = false;

    // Update indicator
    ui_.NameIndicator->setOK(nameValid);

    // Update dialog buttons
    ui_.OKButton->setEnabled(nameValid);
}

void GetGizmoNameDialog::on_CancelButton_clicked(bool checked) { reject(); }

void GetGizmoNameDialog::on_OKButton_clicked(bool checked) { accept(); }
