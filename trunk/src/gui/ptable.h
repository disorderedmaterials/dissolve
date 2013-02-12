/*
	*** Periodic Table
	*** src/gui/ptable.h
	Copyright T. Youngs 2012-2013

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_PTABLE_H
#define DUQ_PTABLE_H

#include "gui/ui_ptable.h"
#include "base/element.h"
#include "templates/reflist.h"
#include <QtGui/QDialog>

// FOrward Declarations
class QPushButton;
class QHBoxLayout;

// Periodic Table 
class dUQPeriodicTable : public QDialog
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	/*
	// Window Functions
	*/
	private:
	QPushButton *addCommonButton(int el);

	private slots:
	void on_CancelButton_clicked(bool checked);
	void ElementButton_clicked(bool checked);

	/*
	// Local variables
	*/
	private:
	// Selected element
	int selectedElement_;
	// Array of element buttons
	QPushButton **elementButtons_;

	/*
	// Methods
	*/
	public:
	// Select an element
	int selectElement();

	/*
	// Widgets
	*/
	public:
	// Constructor / Destructor
	dUQPeriodicTable(QWidget *parent = 0);
	~dUQPeriodicTable();
	// Main form declaration
	Ui::SelectElementDialog ui;
};

#endif
