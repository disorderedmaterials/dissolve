/*
	*** uChroma Data Window
	*** src/gui/uchroma/gui/data.h
	Copyright T. Youngs 2013-2017

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

#ifndef DUQ_UCHROMADATAWINDOW_H
#define DUQ_UCHROMADATAWINDOW_H

#include "gui/uchroma/gui/ui_data.h"

// Forward Declarations
class UChromaBase;

// Data Window
class DataWindow : public QWidget
{
	// All Qt declarations must include this macro
	Q_OBJECT


	/*
	 * Window Functions
	 */
	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;

	public:
	// Constructor / Destructor
	DataWindow(UChromaBase& uChromaBase, QWidget* parent);
	~DataWindow();
	// Main form declaration
	Ui::DataWindow ui;
	// UChromaBase reference
	UChromaBase& uChromaBase_;

	protected:
	// Window close event
	void closeEvent(QCloseEvent* event);

	signals:
	// Window closed signal
	void windowClosed(bool);


	/*
	 * Convenience Functions
	 */
	public:
	// Reload datasets from their associated files
	void reloadDataSets();


	/*
	 * Slots
	 */
	private slots:
	void on_SourceDirSelectButton_clicked(bool checked);
	void on_AddFilesButton_clicked(bool checked);
	void on_RemoveFilesButton_clicked(bool checked);
	void on_DataSetsTable_itemSelectionChanged();
	void on_DataSetsTable_itemDoubleClicked(QTableWidgetItem* item);
	void on_DataSetsTable_cellChanged(int row, int column);
	void on_SetZButton_clicked(bool checked);
	void on_ReloadFilesButton_clicked(bool checked);
	void on_EditDataSetButton_clicked(bool checked);


	/*
	 * Update Functions
	 */
	public:
	// Update controls and show window
	void updateAndShow();
	// Update controls
	void updateControls(bool force = false);
};

#endif
