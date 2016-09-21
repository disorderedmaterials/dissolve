/*
	*** Species DockWidget
	*** src/gui/species.h
	Copyright T. Youngs 2012-2013

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

#ifndef DUQ_SPECIESDIALOG_H
#define DUQ_SPECIESDIALOG_H

#include "gui/ui_species.h"
#include "gui/guiduq.uih"
#include "gui/dwenum.h"

// Forward Declarations
class Isotopologue;
class Sample;
class Species;
class SpeciesGrain;

/** \addtogroup GUI
 * @{
 */

/*
 * \brief Species DockWidget
 * \details TODO
 */
class DUQSpeciesDockWidget : public QDockWidget
{
	// All Qt declarations must include this macro
	Q_OBJECT


	/*
	// Window Functions
	*/
	public:
	// Constructor / Destructor
	DUQSpeciesDockWidget(QWidget* parent, DUQObject& dUQ);
	~DUQSpeciesDockWidget();
	// Main form declaration
	Ui::DUQSpeciesDockWidget ui;
	// Finalise widgets (things that we couldn't do in Qt Designer)
	void finaliseUi();

	protected:
	// Window close event
	void closeEvent(QCloseEvent* event);
	
	public:
	// General refresh function
	void refresh(int targets);


	/*
	 * Variables
	 */
	private:
	// Reference to main dUQ object
	DUQObject& dUQ_;
	// Flag to indicate that the dialog is currently being refreshed
	bool refreshing_;


	/*
	 * Private Functions
	 */
	private:
	// Return current selected Species (if any)
	Species* selectedSpecies();
	// Return current selected GrainDefinition, if any
	SpeciesGrain* selectedGrainDefinition();
	// Return current selected Isotopologue, if any
	Isotopologue* selectedIsotopologue();


	/*
	// Selection Menu
	*/
	private:
	// Transmute selection
// 	void transmuteSelection(int el);
// 
// 	private slots:
// 	void on_actionSelectionHydrogen_triggered(bool checked);
// 	void on_actionSelectionCarbon_triggered(bool checked);
// 	void on_actionSelectionNitrogen_triggered(bool checked);
// 	void on_actionSelectionOxygen_triggered(bool checked);
// 	void on_actionSelectionSilicon_triggered(bool checked);
// 	void on_actionSelectionPhosphorous_triggered(bool checked);
// 	void on_actionSelectionSulfur_triggered(bool checked);
// 	void on_actionSelectionOther_triggered(bool checked);
// 	void on_actionSelectionSelectSame_triggered(bool checked);
// 
// 	public slots:
// 	void selectionAtomTypeChanged(bool checked);
// 	
	/*
	 * Slots
	 */
	private slots:
	// Defined Species Group
	void on_DefinedSpeciesList_currentRowChanged(int row);
	void on_DefinedSpeciesList_itemChanged(QListWidgetItem* item);
	void on_AddSpeciesButton_clicked(bool checked);
	void on_RemoveSpeciesButton_clicked(bool checked);
	void on_LoadSpeciesButton_clicked(bool checked);
	void on_SaveSpeciesButton_clicked(bool checked);

	// Species Viewer
	private slots:
	void on_AddBondFromSelectionButton_clicked(bool checked);
	void on_AddAngleFromSelectionButton_clicked(bool checked);
	void on_AddGrainFromSelectionButton_clicked(bool checked);
	void on_ViewAtomTypeCheck_clicked(bool checked);
	void on_ViewIndexCheck_clicked(bool checked);

	// Grains Group
	private slots:
	void on_GrainsTable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
	void on_GrainsTable_cellChanged(int row, int column);
	void on_AddGrainButton_clicked(bool checked);
	void on_RemoveGrainButton_clicked(bool checked);
	void on_AddDefaultGrainButton_clicked(bool checked);
	void on_AutoGrainButton_clicked(bool checked);
	
	// Isotopologues Group
	private:
	bool getIsotopologueAndAtomType(QStringList data, Isotopologue*& iso, AtomType*& at);
	private slots:
	void on_IsotopologuesTree_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
	void on_IsotopologuesTree_itemChanged(QTreeWidgetItem* item, int column);
	void on_AddIsotopologueToSpeciesButton_clicked(bool checked);
	void on_RemoveIsotopologueFromSpeciesButton_clicked(bool checked);
	void on_CopyIsotopologueButton_clicked(bool checked);
	public slots:
	void isotopologueIsotopeChanged(int index);


	/*
	 * Signals
	 */
	signals:
	void dataChanged(int targets);
};

/** @}*/

#endif
