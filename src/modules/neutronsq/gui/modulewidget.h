/*
	*** Module Widget
	*** src/modules/neutronsq/gui/modulewidget.h
	Copyright T. Youngs 2012-2018

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

#ifndef DUQ_NEUTRONSQMODULEWIDGET_H
#define DUQ_NEUTRONSQMODULEWIDGET_H

#include "modules/neutronsq/gui/ui_modulewidget.h"
#include "gui/modulewidget.h"

// Forward Declarations
class DUQ;
class Module;
class PartialSet;
class UChromaViewWidget;

// Module Widget
class NeutronSQModuleWidget : public ModuleWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	private:
	// Associated Module
	Module* module_;
	// UChromaView contained within this widget
	UChromaViewWidget* uChromaView_;
	// Reference to DUQ
	DUQ& dUQ_;

	public:
	// Constructor / Destructor
	NeutronSQModuleWidget(QWidget* parent, Module* module, DUQ& dUQ);
	~NeutronSQModuleWidget();
	// Main form declaration
	Ui::NeutronSQModuleWidget ui;
	// Update controls within widget
	void updateControls();
	// Disable sensitive controls within widget, ready for main code to run
	void disableSensitiveControls();
	// Enable sensitive controls within widget, ready for main code to run
	void enableSensitiveControls();
	// Data Type Enum
	enum DataType { FullData=1, BoundData=2, UnboundData=3, BraggData=4,
			UnweightedGRData=10, 
			WeightedGRData=20, 
			UnweightedSQData=30, 
			WeightedSQData=40};


	/*
	 * ModuleWidget Implementations
	 */
	public:
	// Write widget state through specified LineParser
	bool writeState(LineParser& parser);
	// Read widget state through specified LineParser
	bool readState(LineParser& parser);


	/*
	 * Widgets / Functions
	 */
	private:
	// Add data from PartialSet to tree
	void addPartialSetToTree(PartialSet& partials, QTreeWidgetItem* topLevelItem, NeutronSQModuleWidget::DataType rootType, const char* rootTitle, int addAverageContributions = 0);
	// Repopulate tree with source data
	void repopulateSourceTree();

	private slots:
	void on_SourcesTree_itemDoubleClicked(QTreeWidgetItem* item, int column);
};

#endif
