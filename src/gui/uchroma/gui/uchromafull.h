/*
	*** uChroma Full Widget
	*** src/gui/uchroma/gui/uchromafull.h
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

#ifndef DUQ_UCHROMAFULLWIDGET_H
#define DUQ_UCHROMAFULLWIDGET_H

#include "gui/uchroma/gui/ui_uchromafull.h"
#include "gui/uchroma/uchromabase.h"
#include "gui/uchroma/classes/collection.h"
#include "gui/uchroma/classes/transformer.h"
#include "gui/uchroma/classes/viewlayout.h"

// Forward Declarations
/* None */

/*
 * uChroma Full Window
 */
class UChromaFullWindow : public QMainWindow, public UChromaBase
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	UChromaFullWindow(QMainWindow *parent = 0);
	~UChromaFullWindow();
	// Main form declaration
	Ui::UChromaFullWindow ui;
	// Return centre coordinate of main window
	QPoint centrePos();

	protected:
	// Window close event
	void closeEvent(QCloseEvent *event);


	public:
	// Load program settings
	void loadSettings();
	// Save program settings
	void saveSettings();


	/*
	 * File Actions
	 */
	private slots:
	void on_actionFileNewSession_triggered(bool checked);
	void on_actionFileLoadSession_triggered(bool checked);
	void on_actionFileSaveSession_triggered(bool checked);
	void on_actionFileSaveSessionAs_triggered(bool checked);
	void on_actionFilePrint_triggered(bool checked);
	void on_actionFileExportImage_triggered(bool checked);
	void on_actionFileQuit_triggered(bool checked);

	private:
	// Check for modified data before closing
	bool checkBeforeClose();


	/*
	 * Edit Actions
	 */
	private slots:
	void on_actionEditUndo_triggered(bool checked);
	void on_actionEditRedo_triggered(bool checked);


	/*
	 * View Actions
	 */
	private:
	// Action group for actions related to ViewPane::ViewType
	QActionGroup viewTypeActionGroup_;

	private:
	bool viewTypeChanged(ViewPane::ViewType vt);

	private slots:
	void on_actionViewPerspective_triggered(bool checked);
	void on_actionViewReset_triggered(bool checked);
	void on_actionViewShowAll_triggered(bool checked);
	void on_actionViewNormal_triggered(bool checked);
	void on_actionViewAutoStretched3D_triggered(bool checked);
	void on_actionViewFlatXY_triggered(bool checked);
	void on_actionViewFlatXZ_triggered(bool checked);
	void on_actionViewFlatZY_triggered(bool checked);
	void on_actionViewLinked_triggered(bool checked);
	void on_actionViewAxes_triggered(bool checked);
	void on_actionViewChangeLayout_triggered(bool checked);


	/*
	 * Collections Actions
	 */
	private slots:
	void on_actionCollectionFocusNext_triggered(bool checked);
	void on_actionCollectionFocusPrevious_triggered(bool checked);
	void on_actionCollectionNew_triggered(bool checked);
	void on_actionCollectionCreate_triggered(bool checked);
	void on_actionCollectionDuplicate_triggered(bool checked);
	void on_actionCollectionPromoteToMaster_triggered(bool checked);
	void on_actionCollectionStyle_triggered(bool checked);
	void on_actionCollectionTransform_triggered(bool checked);
	void on_actionCollectionDelete_triggered(bool checked);
	void on_actionCollectionExport_triggered(bool checked);


	/*
	 * Data Actions
	 */
	private slots:
	void on_actionDataLoadXY_triggered(bool checked);
	void on_actionDataImport_triggered(bool checked);
	void on_actionDataReload_triggered(bool checked);
	void on_actionDataView_triggered(bool checked);


	/*
	 * Operate Actions
	 */
	private slots:
	void on_actionOperateConstantBackgroundSubtraction_triggered(bool checked);
	void on_actionOperateSetZValues_triggered(bool checked);


	/*
	 * Analyse Actions
	 */
	private slots:
	void on_actionAnalyseNewFit_triggered(bool checked);
	void on_actionAnalyseEditFit_triggered(bool checked);
	void on_actionAnalyseUpdateFit_triggered(bool checked);
	void on_actionAnalyseResetAndRestartFit_triggered(bool checked);
	void on_actionAnalyseViewFitResults_triggered(bool checked);


	/*
	 * Settings Menu
	 */
	private slots:
	void on_actionSettingsChooseFont_triggered(bool checked);
	void on_actionSettingsResetFont_triggered(bool checked);


	/*
	 * Interact Actions
	 */
	private:
	// Change interaction axis target
	void interactionActionTriggered(int axis);

	private slots:
	void on_actionInteractX_triggered(bool checked);
	void on_actionInteractY_triggered(bool checked);
	void on_actionInteractZ_triggered(bool checked);
	void on_actionInteractNone_triggered(bool checked);


	/*
	 * Window Actions
	 */
	public slots:
	void on_actionWindowAxes_triggered(bool checked);
	void on_actionWindowData_triggered(bool checked);
	void on_actionWindowStyle_triggered(bool checked);
	void on_actionWindowTransform_triggered(bool checked);
	void on_actionWindowView_triggered(bool checked);


	/*
	 * Collections
	 */
	private:
	// Update data for specified tree item
	void updateCollectionTreeItem(QTreeWidgetItem* item);
	// Add collection data to CollectionTree under specified item
	void addCollectionsToTree(Collection* collection, QTreeWidgetItem* parent);

	private slots:
	void on_CollectionListButton_clicked(bool checked);
	void on_CollectionFocusNextButton_clicked(bool checked);
	void on_CollectionFocusPreviousButton_clicked(bool checked);
	void on_CollectionTree_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
	void on_CollectionTree_itemClicked(QTreeWidgetItem* item, int column);
	void on_CollectionTree_itemDoubleClicked(QTreeWidgetItem* item, int column);
	void on_CollectionTree_itemChanged(QTreeWidgetItem* item, int column);
	void on_CollectionAddButton_clicked(bool checked);
	void on_CollectionRemoveButton_clicked(bool checked);

	public slots:
	// Context menu requested for CollectionTree
	void collectionTreeContextMenuRequested(const QPoint& point);
	// Refresh collection list
	void refreshCollections();
	// Update displayed info for current collection
	void updateCollectionInfo();


	/*
	 * Update (satisfying virtuals in UChromaBase)
	 */
	private:
	// Statusbar info line label
	QLabel* statusBarInfoLabel_;
	// Statusbar rendering time label
	QLabel* statusBarRenderingTimeLabel_;

	public slots:
	// Update all aspects of GUI
	void updateGUI();
	// Update controls in all subwindows
	void updateSubWindows();
	// Update tool bars
	void updateToolBars();
	// Update main display
	void updateDisplay();
	// Update undo/redo menu
	void updateUndoRedo();
	// Update coordinate info
	void updateCoordinateInfo();
	// Update text of renderTimeLabel_ in statusbar
	void updateRenderTimeLabel(QString text);
	// Update title bar of main window
	void updateTitleBar();
};

#endif
