/*
	*** Get Workspace Name Dialog
	*** src/gui/getworkspacenamedialog_funcs.cpp
	Copyright T. Youngs 2012-2019

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

#include "gui/getworkspacenamedialog.h"
#include "gui/workspacetab.h"
#include "base/sysfunc.h"

// Constructor
GetWorkspaceNameDialog::GetWorkspaceNameDialog(QWidget* parent, const List<WorkspaceTab>& currentWorkspaces) : currentWorkspaces_(currentWorkspaces)
{
	ui_.setupUi(this);
}

// Destructor
GetWorkspaceNameDialog::~GetWorkspaceNameDialog()
{
}

// Run the dialog
bool GetWorkspaceNameDialog::get(const WorkspaceTab* workspace, const char* currentName)
{
	workspace_ = workspace;

	// Set the current text
	ui_.NameEdit->setText(currentName);

	show();

	return (exec() == QDialog::Accepted);
}

// Return the name string
QString GetWorkspaceNameDialog::newName() const
{
	return ui_.NameEdit->text();
}

/*
 * Slots
 */

// Workspace name edited
void GetWorkspaceNameDialog::on_NameEdit_textChanged(const QString text)
{
	// Make sure the name is valid
	bool nameValid = true;
	if (text.isEmpty()) nameValid = false;
	else
	{
		ListIterator<WorkspaceTab> workspaceIterator(currentWorkspaces_);
		while (WorkspaceTab* tab = workspaceIterator.iterate())
		{
			if (workspace_ == tab) continue;

			if (DissolveSys::sameString(tab->title(), qPrintable(text)))
			{
				nameValid = false;
				break;
			}
		}
	}

	// Update indicator
	ui_.NameIndicator->setOK(nameValid);

	// Update dialog buttons
	ui_.OKButton->setEnabled(nameValid);
}

void GetWorkspaceNameDialog::on_CancelButton_clicked(bool checked)
{
	reject();
}

void GetWorkspaceNameDialog::on_OKButton_clicked(bool checked)
{
	accept();
}
