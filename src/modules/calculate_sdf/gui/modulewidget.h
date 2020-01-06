/*
	*** Module Widget
	*** src/modules/calculate_sdf/gui/modulewidget.h
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

#ifndef DISSOLVE_MODULEWIDGET_CALCULATESDF_H
#define DISSOLVE_MODULEWIDGET_CALCULATESDF_H

#include "modules/calculate_sdf/gui/ui_modulewidget.h"
#include "gui/modulewidget.h"

// Forward Declarations
class CalculateSDFModule;
class CoreData;
class DataViewer;
class RenderableData3D;
class RenderableSpecies;
class Species;

// Module Widget
class CalculateSDFModuleWidget : public ModuleWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor
	CalculateSDFModuleWidget(QWidget* parent, CalculateSDFModule* module, const CoreData& coreData);

	private:
	// Associated Module
	CalculateSDFModule* module_;
	// CoreData reference
	const CoreData& coreData_;

	/*
	 * UI
	 */
	private:
	// Main form declaration
	Ui::CalculateSDFModuleWidget ui_;
	// DataViewers contained within this widget
	DataViewer* sdfGraph_;
	// Renderable for our SDF
	RenderableData3D* sdfRenderable_;
	// Reference molecule (Species) to display alongside density
	Species* referenceMolecule_;
	// Renderable for reference molecule
	RenderableSpecies* referenceMoleculeRenderable_;

	public:
	// Update controls within widget
	void updateControls(int flags = ModuleWidget::DefaultUpdateFlag);


	/*
	 * State I/O
	 */
	public:
	// Write widget state through specified LineParser
	bool writeState(LineParser& parser) const;
	// Read widget state through specified LineParser
	bool readState(LineParser& parser);


	/*
	 * Widgets / Functions
	 */
	private:
	// Set data targets in graphs
	void setGraphDataTargets();

	private slots:
	void on_LowerCutoffSpin_valueChanged(double value);
	void on_UpperCutoffSpin_valueChanged(double value);
	void on_ReferenceMoleculeCombo_currentIndexChanged(int index);
};

#endif
