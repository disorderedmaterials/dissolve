/*
	*** Data Import Dialog
	*** src/gui/uchroma/gui/import.h
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

#ifndef DUQ_UCHROMAIMPORT_H
#define DUQ_UCHROMAIMPORT_H

#include "gui/uchroma/gui/ui_import.h"
#include "gui/uchroma/classes/dataset.h"

// Forward Declarations
class UChromaBase;

class ImportDialog : public QDialog
{
	// All Qt declarations must include this macro
	Q_OBJECT


	/*
	// Window Functions
	*/
	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;

	public:
	// Constructor / Destructor
	ImportDialog(UChromaBase& uChromaBase, QWidget* parent);
	~ImportDialog();
	// Main form declaration
	Ui::ImportDialog ui;
	// UChromaBase reference
	UChromaBase& uChromaBase_;

	protected:
	// Window close event
	void closeEvent(QCloseEvent *event);


	/*
	 * Import Functions
	 */
	private:
	// Current directory for file selector
	QDir currentDirectory_;
	// List of imported datasets
	List<DataSet> importedDataSets_;

	private:
	// Import sequential XY data
	bool importSequentialXY();

	public:
	// Run the import, showing the dialog
	bool import();
	// Return first imported slice
	DataSet* importedSlices();
	// Return selected filename
	QString filename();
	// Return whether a new collection should be created for the imported data
	bool createNewCollection();


	/*
	 * Private Slots
	 */
	private slots:
	// -- Global Widgets
	void on_DataFileSelectButton_clicked(bool checked);
	void on_DataFileEdit_textChanged(QString text);
	void on_ImportButton_clicked(bool checked);
	void on_CancelButton_clicked(bool checked);
};

#endif
