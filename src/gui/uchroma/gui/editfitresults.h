/*
	*** Edit Fit Results Dialog
	*** src/gui/uchroma/gui/editfitresultsdialog.h
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

#ifndef DUQ_UCHROMAEDITFITRESULTSDIALOG_H
#define DUQ_UCHROMAEDITFITRESULTSDIALOG_H

#include "gui/uchroma/gui/ui_editfitresults.h"
#include <QDialog>

// Forward Declarations
class Collection;

class EditFitResultsDialog : public QDialog
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	EditFitResultsDialog(QWidget *parent);
	~EditFitResultsDialog();
	// Main form declaration
	Ui::EditFitResultsDialog ui;


	/*
	 * Window Functions
	 */
	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;
	// Collection in which fitted values are probed
	Collection* collection_;

	protected:
	// Window close event
	void closeEvent(QCloseEvent* event);

	public slots:
	void reject();
	void on_CloseButton_clicked(bool checked);

	public:
	// Call dialog to show info from specified Collection
	bool call(Collection* collection);


	/*
	 * Slots
	 */
	private slots:
	//void on_FitResultsTable_
};

#endif
