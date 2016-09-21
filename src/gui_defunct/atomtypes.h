/*
	*** AtomTypes DockWidget
	*** src/gui/atomtypes.h
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

#ifndef DUQ_ATOMTYPESDIALOG_H
#define DUQ_ATOMTYPESDIALOG_H

#include "gui/ui_atomtypes.h"
#include "gui/guiduq.uih"
#include "gui/doublespindelegate.uih"

// Forward Declarations
/* none */

/** \addtogroup GUI
 * @{
 */

/*
 * \brief AtomTypes DockWidget
 * \details TODO
 */
class DUQAtomTypesDockWidget : public QDockWidget
{
	// All Qt declarations must include this macro
	Q_OBJECT


	/*
	// Window Functions
	*/
	public:
	// Constructor / Destructor
	DUQAtomTypesDockWidget(QWidget* parent, DUQObject& dUQ);
	~DUQAtomTypesDockWidget();
	// Main form declaration
	Ui::DUQAtomTypesDockWidget ui;
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
	// Return current selected AtomType, if any
	AtomType* selectedAtomType();


	/*
	 * Slots
	 */
	private slots:
	void on_DefinedAtomTypesTree_itemClicked(QTreeWidgetItem* item, int column);
	void on_DefinedAtomTypesTree_itemChanged(QTreeWidgetItem* item, int column);
	void on_AddAtomTypeButton_clicked(bool checked);
	void on_CopyAtomTypeButton_clicked(bool checked);
	void on_RemoveAtomTypeButton_clicked(bool checked);

	public slots:
	void atomTypeParametersChanged(int index);


	/*
	 * Signals
	 */
	signals:
	void dataChanged(int targets);
};

/** @}*/

#endif
