/*
	*** Select Forcefield Widget
	*** src/gui/selectforcefieldwidget.h
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

#ifndef DISSOLVE_WIDGET_SELECTFORCEFIELD_H
#define DISSOLVE_WIDGET_SELECTFORCEFIELD_H

#include "gui/ui_selectforcefieldwidget.h"
#include "data/fflibrary.h"
#include "templates/list.h"
#include <QWidget>

// Forward Declarations
class Dissolve;
class Forcefield;

// Select Forcefield Widget
class SelectForcefieldWidget : public QWidget
{
	Q_OBJECT

	public:
	// Constructor
	SelectForcefieldWidget(QWidget* parent, const List<Forcefield>& forcefields = ForcefieldLibrary::forcefields());
	// Destructor
	~SelectForcefieldWidget();

	private:
	// Main form declaration
	Ui::SelectForcefieldWidget ui_;
	// Available forcefields
	const List<Forcefield>& forcefields_;
	// Whether the widget is refreshing
	bool refreshing_;

	private:
	// Update the list of Forcefields, optionally filtering them by name and description
	void updateForcefieldsList(Forcefield* current = NULL, QString filter = QString());

	private slots:
	void on_FilterEdit_textChanged(const QString& text);
	void on_ForcefieldsList_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
	void on_ForcefieldsList_itemDoubleClicked(QListWidgetItem* item);

	signals:
	void forcefieldSelectionChanged(bool isValid);
	void forcefieldDoubleClicked();

	public:
	// Set the current forcefield
	void setCurrentForcefield(Forcefield* currentFF);
	// Return the currently-selected Forcefield
	Forcefield* currentForcefield() const;
};

#endif
