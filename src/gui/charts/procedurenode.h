/*
	*** Procedure Chart Node Block
	*** src/gui/charts/procedurenode.h
	Copyright T. Youngs 2012-2019

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

#ifndef DISSOLVE_CHARTS_PROCEDURE_NODE_H
#define DISSOLVE_CHARTS_PROCEDURE_NODE_H

#include "gui/charts/ui_procedurenode.h"
#include "gui/charts/chartblock.h"
#include "templates/reflist.h"

// Forward Declarations
class KeywordWidgetBase;
class ProcedureNode;

// Procedure Charte Node Block Widget
class ProcedureChartNodeBlock : public QWidget, public ChartBlock
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	ProcedureChartNodeBlock(QWidget* parent, ProcedureNode* node);
	~ProcedureChartNodeBlock();
	// Main form declaration
	Ui::ModuleChartModuleWidget ui;


	/*
	 * Node Target
	 */
	private:
	// Displayed node
	ProcedureNode* node_;
	// List of keyword widgets displayed
	RefList<KeywordWidgetBase> keywordWidgets_;
	// Widgets that exist in the branch of our Procedure node
	RefList<ProcedureChartNodeBlock> branchWidgets_;

	public:
	// Return displayed node
	ProcedureNode* node() const;
	// Set whether the settings are expanded or not, and whether this is permanent
	void setSettingsExpanded(bool expanded, bool permanent = false);
	// Hide the remove button
	void hideRemoveButton();
	// Return RefList of widgets that exist in the branch of our Procedure node
	RefList<ProcedureChartNodeBlock>& branchWidgets();

	public slots:
	void on_ToggleSettingsButton_clicked(bool checked);
	void on_RemoveButton_clicked(bool checked);

	signals:
	void settingsToggled();
	void remove(QString myUniqueName);


	/*
	 * QWidget Reimplementations
	 */
	protected:
	// Paint event
	void paintEvent(QPaintEvent* event);


	/*
	 * Type (ChartBlock Reimplementations)
	 */
	public:
	// Return type of this block
	const char* blockType();


	/*
	 * Widget (ChartBlock Reimplementations)
	 */
	public:
	// Return underlying widget
	QWidget* widget();
	// Return width of underlying widget
	int widgetWidth() const;
	// Return height of underlying widget
	int widgetHeight() const;
	// Set underlying widget position
	void setWidgetPosition(int left, int top);
	// Set underlying widget geometry
	void setWidgetGeometry(int left, int top, int width, int height);
	// Return whether the supplied point (in local widget coordinates) allows a drag operation to begin
	bool isDragPoint(QPoint point) const;


	/*
	 * Update (ChartBlock Reimplementations)
	 */
	public:
	// Update controls within widget
	void updateControls();
	// Disable sensitive controls
	void disableSensitiveControls();
	// Enable sensitive controls
	void enableSensitiveControls();
};

#endif
