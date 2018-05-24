/*
	*** Edit LineStyle Dialog
	*** src/gui/uchroma/gui/editlinestyledialog.h
	Copyright T. Youngs 2013-2018

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

#ifndef DISSOLVE_UCHROMAEDINUMBERFORMATDIALOG_H
#define DISSOLVE_UCHROMAEDINUMBERFORMATDIALOG_H

#include "gui/uchroma/gui/ui_editnumberformat.h"
#include "gui/uchroma/classes/numberformat.h"
#include <QDialog>

// Forward Declarations
/* none */

class EditNumberFormatDialog : public QDialog
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	EditNumberFormatDialog(QWidget *parent);
	~EditNumberFormatDialog();
	// Main form declaration
	Ui::EditNumberFormatDialog ui;


	/*
	 * Window Functions
	 */
	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;
	// LineStyle being edited
	NumberFormat numberFormat_;

	protected:
	// Window close event
	void closeEvent(QCloseEvent* event);

	public slots:
	void reject();
	void on_OKButton_clicked(bool checked);
	void on_CancelButton_clicked(bool checked);

	public:
	// Call dialog to edit specified NumberFormat
	bool call(NumberFormat target);
	// Return NumberFormat stored in dialog
	NumberFormat& numberFormat();


	/*
	 * Slots
	 */
	private slots:
	void on_IntegerFormatRadio_toggled(bool checked);
	void on_DecimalFormatRadio_toggled(bool checked);
	void on_ScientificFormatRadio_toggled(bool checked);
	void on_DecimalsSpin_valueChanged(int value);
	void on_UpperCaseExponentCheck_clicked(bool checked);
	void on_UseENotationCheck_clicked(bool checked);
	void on_PrecedeWithPlusCheck_clicked(bool checked);

	private:
	// Update preview label
	void updatePreview();
};

#endif
