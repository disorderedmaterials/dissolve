/*
	*** Edit LineStyle Dialog
	*** src/gui/uchroma/gui/editlinestyle_funcs.cpp
	Copyright T. Youngs 2013-2019

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

#include "gui/uchroma/gui/editlinestyle.h"
#include "base/messenger.h"
#include <QPainter>

// Constructor
EditLineStyleDialog::EditLineStyleDialog(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);
}

// Destructor
EditLineStyleDialog::~EditLineStyleDialog()
{
}

/*
 * Window Functions
 */

// Window close event
void EditLineStyleDialog::closeEvent(QCloseEvent* event)
{
	reject();
}

void EditLineStyleDialog::reject()
{
	setResult(QDialog::Rejected);
	hide();
}

void EditLineStyleDialog::on_OKButton_clicked(bool checked)
{
	accept();
}

void EditLineStyleDialog::on_CancelButton_clicked(bool checked)
{
	reject();
}

// Call dialog to edit specified reference
bool EditLineStyleDialog::call(LineStyle target)
{
	// Copy supplied data
	lineStyle_ = target;

	refreshing_ = true;

	// Create entries in LineStippleCombo
	for (int n=0; n<LineStipple::nStippleTypes; ++n) LineStipple::stipple[n].addStippleItem(ui.LineStippleCombo, 3);

	// Update widgets
	ui.LineWidthSpin->setValue(lineStyle_.width());
	ui.LineColourButton->setColour(lineStyle_.colour());
	ui.LineStippleCombo->setCurrentIndex(lineStyle_.stipple());

	refreshing_ = false;

	// Call the dialog
	if (exec()) return true;
	else return false;
}

// Return edited data
LineStyle EditLineStyleDialog::lineStyle()
{
	return lineStyle_;
}

/*
 * Slots
 */

void EditLineStyleDialog::on_LineColourButton_clicked(bool checked)
{
	if (refreshing_) return;

	if (ui.LineColourButton->selectColour()) lineStyle_.setColour(ui.LineColourButton->colour());
}

void EditLineStyleDialog::on_LineWidthSpin_valueChanged(double value)
{
	if (refreshing_) return;

	lineStyle_.setWidth(value);
}

void EditLineStyleDialog::on_LineStippleCombo_currentIndexChanged(int currentIndex)
{
	if (refreshing_) return;

	lineStyle_.setStipple( (LineStipple::StippleType) currentIndex);
}