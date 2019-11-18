/*
	*** Get Workspace Name Dialog
	*** src/gui/getworkspacenamedialog.h
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

#ifndef DISSOLVE_DIALOG_GETWORKSPACENAME_H
#define DISSOLVE_DIALOG_GETWORKSPACENAME_H

#include "gui/ui_getworkspacenamedialog.h"
#include "templates/list.h"
#include <QDialog>

// Forward Declarations
class CoreData;
class WorkspaceTab;

// Get Workspace Name Dialog
class GetWorkspaceNameDialog : public QDialog
{
	Q_OBJECT

	public:
	// Constructor
	GetWorkspaceNameDialog(QWidget* parent, const List<WorkspaceTab>& currentWorkspaces);
	// Destructor
	~GetWorkspaceNameDialog();

	private:
	// Main form declaration
	Ui::GetWorkspaceNameDialog ui_;
	// List of current workspaces
	const List<WorkspaceTab>& currentWorkspaces_;
	// Current Workspace that we are renaming
	const WorkspaceTab* workspace_;

	public:
	// Run the dialog
	bool get(const WorkspaceTab* currentWorkspace, const char* currentName);
	// Return the name string
	QString newName() const;


	/*
	 * Slots
	 */
	private slots:
	void on_NameEdit_textChanged(const QString text);
	void on_CancelButton_clicked(bool checked);
	void on_OKButton_clicked(bool checked);
};

#endif
