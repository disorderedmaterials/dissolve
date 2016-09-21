/*
	*** System DockWidget
	*** src/gui/system.h
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

#ifndef DUQ_SYSTEMDIALOG_H
#define DUQ_SYSTEMDIALOG_H

#include "gui/ui_system.h"
#include "gui/guiduq.uih"
#include "gui/doublespindelegate.uih"

// Forward Declarations
class Sample;
class Species;
class Isotopologue;

/** \addtogroup GUI
 * @{
 */

/*
 * \brief System DockWidget
 * \details TODO
 */
class DUQSystemDockWidget : public QDockWidget
{
	// All Qt declarations must include this macro
	Q_OBJECT


	/*
	// Window Functions
	*/
	public:
	// Constructor / Destructor
	DUQSystemDockWidget(QWidget* parent, DUQObject& dUQ);
	~DUQSystemDockWidget();
	// Main form declaration
	Ui::DUQSystemDockWidget ui;
	// Finalise widgets (things that we couldn't do in Qt Designer)
	void finaliseUi();

	protected:
	// Window close event
	void closeEvent(QCloseEvent* event);
	
	public slots:
	// General refresh function
	void refresh(int targets);


	/*
	 * Variables
	 */
	private:
	// Reference to main dUQ object
	DUQObject& dUQ_;
	// Delegates for ComponentsTable
	DoubleSpinDelegate* speciesRelativePopulationDelegate_;
	// Flag to indicate that the dialog is currently being refreshed
	bool refreshing_;


	/*
	 * Private Functions
	 */
	private:


	/*
	 * Slots
	 */
	private slots:
	void on_CompositionTable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
	void on_CompositionTable_cellChanged(int row, int column);
	void on_MultiplierSpin_valueChanged(int value);
	void on_DensitySpin_valueChanged(double value);
	void on_DensityUnitsCombo_currentIndexChanged(int index);
	void on_BoxRelativeASpin_valueChanged(double value);
	void on_BoxRelativeBSpin_valueChanged(double value);
	void on_BoxRelativeCSpin_valueChanged(double value);
	void on_BoxAlphaSpin_valueChanged(double value);
	void on_BoxBetaSpin_valueChanged(double value);
	void on_BoxGammaSpin_valueChanged(double value);

	public slots:
};

/** @}*/

#endif
