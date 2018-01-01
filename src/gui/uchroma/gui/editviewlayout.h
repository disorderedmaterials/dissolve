/*
	*** Edit ViewLayout Dialog
	*** src/gui/uchroma/gui/    editviewlayout.h
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

#ifndef DUQ_UCHROMAEDITVIEWLAYOUTDIALOG_H
#define DUQ_UCHROMAEDITVIEWLAYOUTDIALOG_H

#include "gui/uchroma/gui/ui_editviewlayout.h"
#include "gui/uchroma/classes/viewlayout.h"

// Forward Declarations
class UChromaBase;

class EditViewLayoutDialog : public QDialog
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	EditViewLayoutDialog(UChromaBase& uchromaBase, QWidget* parent);
	~EditViewLayoutDialog();
	// Main form declaration
	Ui::EditViewLayoutDialog ui;
	// UChromaBase reference
	UChromaBase& uChromaBase_;


	/*
	 * Window Functions
	 */
	private:
	// Whether the window is refreshing / updating its controls
	bool refreshing_;
	// ViewLayout we are editing
	ViewLayout layout_;

	protected:
	// Window close event
	void closeEvent(QCloseEvent *event);

	public slots:
	void reject();
	void on_OKButton_clicked(bool checked);
	void on_CancelButton_clicked(bool checked);

	// Call dialog to edit specified ViewLayout
	bool call(ViewLayout target);
	// Return ViewLayout stored in dialog
	ViewLayout& viewLayout();


	/*
	 * Current Pane
	 */
	private:
	// Current pane selected in PaneOrganiser
	ViewPane* currentPane_;

	public:
	// Return current pane
	ViewPane* currentPane();


	/*
	 * Convenience Functions
	 */
	private:


	/*
	 * Slots
	 */
	private slots:
	// Main Layout
	void on_GridNColumnsSpin_valueChanged(int value);
	void on_GridNRowsSpin_valueChanged(int value);
	void on_PaneAddButton_clicked(bool checked);
	void on_PaneRemoveButton_clicked(bool checked);
	void on_PaneBringToFrontButton_clicked(bool checked);
	void on_PaneSendToBackButton_clicked(bool checked);
	void on_Organiser_currentPaneChanged(int gridX, int gridY);
	// Current Pane
	void on_PaneNameEdit_textChanged(QString text);
	void on_PaneRoleCombo_currentIndexChanged(int index);
	void on_PaneNextButton_clicked(bool checked);
	void on_PanePreviousButton_clicked(bool checked);
	// Pane View Style
	void on_PaneViewTypeCombo_currentIndexChanged(int index);
	// Axis Scaling
	void on_PaneAutoScaleXCombo_currentIndexChanged(int index);
	void on_PaneAutoScaleYCombo_currentIndexChanged(int index);
	void on_PaneAutoScaleZCombo_currentIndexChanged(int index);
	// Pane Targets
	void on_PaneTargetsList_currentRowChanged(int index);
	void on_PaneAddTargetButton_clicked(bool checked);
	void on_PaneRemoveTargetButton_clicked(bool checked);


	/*
	 * Update Functions
	 */
	public:
	// Update controls
	void updateControls();
};

#endif
