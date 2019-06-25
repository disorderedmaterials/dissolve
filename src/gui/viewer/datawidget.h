/*
	*** Data Widget
	*** src/gui/viewer/datawidget.h
	Copyright T. Youngs 2013-2019

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

#ifndef DISSOLVE_DATAWIDGET_H
#define DISSOLVE_DATAWIDGET_H

#include "gui/viewer/ui_datawidget.h"

// Forward Declarations
/* none */

// Data Widget
class DataWidget : public QWidget
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	DataWidget(QWidget* parent = 0);
	~DataWidget();

	private:
	// Main form declaration
	Ui::DataWidget ui_;

	public:
	// Return contained DataViewer
	DataViewer* dataViewer();


	/*
	 * Tools
	 */
	private slots:
	// Interaction
	void on_InteractionViewButton_clicked(bool checked);
	// View
	void on_ViewResetButton_clicked(bool checked);
	void on_ViewAxesVisibleButton_clicked(bool checked);
	void on_ViewCopyToClipboardButton_clicked(bool checked);
	void on_ViewFollowAllButton_clicked(bool checked);
	void on_ViewFollowXButton_clicked(bool checked);
	void on_ViewFollowXLengthSpin_valueChanged(double value);
	// Data
	void on_DataToggleTreeButton_clicked(bool checked);


	/*
	 * Update Functions
	 */
	private:
	// Whether the widget is currently refreshing
	bool refreshing_;

	private:
	// Data tree top-level item update function
	void dataTreeTopLevelUpdateFunction(QTreeWidget* treeWidget, int topLevelItemIndex, RenderableGroup* data, bool createItem);
	// Data tree item update function
	void dataTreeItemUpdateFunction(QTreeWidgetItem* parentItem, int childIndex, Renderable* data, bool createItem);

	public slots:
	// Update toolbar
	void updateToolbar();
	// Update status bar
	void updateStatusBar();
	// Update data tree
	void updateDataTree();
};

#endif
