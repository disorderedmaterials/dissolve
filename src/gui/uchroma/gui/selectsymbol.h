/*
	*** Select Symbol Dialog
	*** src/gui/uchroma/gui/selectsymbol.h
	Copyright T. Youngs 2012-2018.

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DISSOLVE_UCHROMASELECTSYMBOL_H
#define DISSOLVE_UCHROMASELECTSYMBOL_H

#include "gui/uchroma/gui/ui_selectsymbol.h"
#include <QDialog>
#include <QObject>

// Forward Declarations
/* none */

/*
 * Select Symbol Dialog
 */
class SelectSymbolDialog : public QDialog
{
	Q_OBJECT

	public:
	// Constructor
	SelectSymbolDialog(QWidget *parent);
	// Destructor
	~SelectSymbolDialog();
	// Main form declaration
	Ui::SelectSymbolDialog ui;


	/*
	 * Symbol Table
	 */
	private:
	// Desired minimum size of item in table
	const int itemSize_;
	// Selected equation
	QChar selectedSymbol_;

	public:
	// Return selected character
	QChar selectedSymbol();


	/*
	 * Reimplementations
	 */
	protected:
	void resizeEvent(QResizeEvent* event);


	/*
	 * Slots
	 */
	public slots:
	void on_SearchEdit_textChanged(QString text);
	void on_ClearSearchButton_clicked(bool checked);
	void on_SymbolTable_itemSelectionChanged();
	void on_SymbolTable_itemDoubleClicked(QTableWidgetItem* item);
	void on_CancelButton_clicked(bool checked);
	void on_SelectButton_clicked(bool checked);


	/*
	 * Update
	 */
	public:
	// Update symbol table
	void updateTable(bool force = false);
};

#endif
