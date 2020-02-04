/*
	*** Select System Template Dialog
	*** src/gui/selectsystemtemplatedialog.h
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

#ifndef DISSOLVE_DIALOG_SELECTSYSTEMTEMPLATE_H
#define DISSOLVE_DIALOG_SELECTSYSTEMTEMPLATE_H

#include "gui/ui_selectsystemtemplatedialog.h"
#include "templates/list.h"
#include <QDialog>

// Forward Declarations
class Dissolve;
class SystemTemplate;

// Select System Template Dialog
class SelectSystemTemplateDialog : public QDialog
{
	Q_OBJECT

	public:
	// Constructor
	SelectSystemTemplateDialog(QWidget* parent, const List<SystemTemplate>& systemTemplates);
	// Destructor
	~SelectSystemTemplateDialog();

	private:
	// Main form declaration
	Ui::SelectSystemTemplateDialog ui_;
	// Available system templates
	const List<SystemTemplate>& systemTemplates_;

	private:
	// Update the list of system templates, optionally filtering them by name and description
	void updateTemplatesList(QString filter = QString());

	private slots:
	void on_FilterEdit_textChanged(const QString& text);
	void on_TemplatesList_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
	void on_TemplatesList_itemDoubleClicked(QListWidgetItem* item);
	void on_SelectButton_clicked(bool checked);
	void on_CancelButton_clicked(bool checked);

	public:
	// Run the dialog, returning the selected SystemTemplate
	SystemTemplate* selectTemplate();
};

#endif
