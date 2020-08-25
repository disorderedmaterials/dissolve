/*
    *** Get Gizmo Name Dialog
    *** src/gui/getgizmonamedialog_funcs.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    // 	else if (WorkspaceTab::findGizmo(qPrintable(text), currentGizmo_)) nameValid = false;

    // Update indicator
    ui_.NameIndicator->setOK(nameValid);

    // Update dialog buttons
    ui_.OKButton->setEnabled(nameValid);
}

void GetGizmoNameDialog::on_CancelButton_clicked(bool checked) { reject(); }

void GetGizmoNameDialog::on_OKButton_clicked(bool checked) { accept(); }
