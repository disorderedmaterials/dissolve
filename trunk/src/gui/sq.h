/*
	*** S(Q) DockWidget
	*** src/gui/sq.h
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

#ifndef DUQ_SQDIALOG_H
#define DUQ_SQDIALOG_H

#include "gui/ui_sq.h"
#include "gui/guiduq.uih"

/** \addtogroup GUI
 *  @{
 */

/*!
 * \brief S(Q) DockWidget
 * \details TODO
 */
class DUQSQDockWidget : public QDockWidget
{
	// All Qt declarations must include this macro
	Q_OBJECT


	/*
	// Window Functions
	*/
	public:
	// Constructor / Destructor
	DUQSQDockWidget(QWidget* parent, DUQObject& dUQ);
	~DUQSQDockWidget();
	// Main form declaration
	Ui::DUQSQDockWidget ui;
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
	 * Slots
	 */
	private:

	private slots:
};

/** @}*/

#endif
