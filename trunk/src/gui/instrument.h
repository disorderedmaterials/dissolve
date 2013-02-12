/*
	*** Instrument Dialog
	*** src/gui/instrument.h
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

#ifndef DUQ_INSTRUMENTDIALOG_H
#define DUQ_INSTRUMENTDIALOG_H

#include "gui/ui_instrument.h"

// Forward Declarations
class Instrument;

class InstrumentDialog : public QDialog
{
	// All Qt declarations must include this macro
	Q_OBJECT


	/*
	// Window Functions
	*/
	public:
	// Constructor / Destructor
	InstrumentDialog(QWidget* parent = 0);
	~InstrumentDialog();
	// Main form declaration
	Ui::InstrumentDialog ui;
	// Slot column enum
	enum SlotColumns { SlotName, SlotLongName, SlotDistance, SlotPolarAngle, SlotAzimuthalAngle, nSlotColumns };

	private:
	// Source instrument pointer
	Instrument* sourceInstrument_;

	private slots:
	// Dialog accepted
	void accept();

	/*
	// Detector Module Slot List
	*/
	private slots:
	void on_AddSlotButton_clicked(bool checked);
	void on_DeleteSlotButton_clicked(bool checked);
	void on_CloneSlotButton_clicked(bool checked);

	/*
	// Detector Module Types List
	*/
	private slots:
	void on_AddTypeButton_clicked(bool checked);
	void on_DeleteTypeButton_clicked(bool checked);
	void on_CloneTypeButton_clicked(bool checked);
};

#endif
