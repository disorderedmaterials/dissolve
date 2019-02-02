/*
	*** Edit NumberFormat Dialog
	*** src/gui/uchroma/gui/editnumberformat_funcs.cpp
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

#include "gui/uchroma/gui/editnumberformat.h"
#include "base/messenger.h"
#include <QPainter>

// Constructor
EditNumberFormatDialog::EditNumberFormatDialog(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);
}

// Destructor
EditNumberFormatDialog::~EditNumberFormatDialog()
{
}

/*
 * Window Functions
 */

// Window close event
void EditNumberFormatDialog::closeEvent(QCloseEvent* event)
{
	reject();
}

void EditNumberFormatDialog::reject()
{
	setResult(QDialog::Rejected);
	hide();
}

void EditNumberFormatDialog::on_OKButton_clicked(bool checked)
{
	accept();
}

void EditNumberFormatDialog::on_CancelButton_clicked(bool checked)
{
	reject();
}

// Call dialog to edit specified NumberFormat
bool EditNumberFormatDialog::call(NumberFormat target)
{
	// Copy supplied data
	numberFormat_ = target;

	refreshing_ = true;

	if (numberFormat_.type() == NumberFormat::IntegerFormat) ui.IntegerFormatRadio->setChecked(true);
	else if (numberFormat_.type() == NumberFormat::DecimalFormat) ui.DecimalFormatRadio->setChecked(true);
	else if (numberFormat_.type() == NumberFormat::ScientificFormat) ui.ScientificFormatRadio->setChecked(true);

	ui.DecimalsSpin->setValue(numberFormat_.nDecimals());
	ui.PrecedeWithPlusCheck->setChecked(numberFormat_.forcePrecedingPlus());
	ui.UpperCaseExponentCheck->setChecked(numberFormat_.useUpperCaseExponent());

	refreshing_ = false;

	// Call the dialog
	if (exec()) return true;
	else return false;
}

// Return NumberFormat stored in dialog
NumberFormat& EditNumberFormatDialog::numberFormat()
{
	return numberFormat_;
}

/*
 * Slots
 */

void EditNumberFormatDialog::on_IntegerFormatRadio_toggled(bool checked)
{
	if (!checked) return;

	ui.DecimalsSpin->setEnabled(false);

	if (refreshing_) return;

	numberFormat_.setType(NumberFormat::IntegerFormat);

	updatePreview();
}

void EditNumberFormatDialog::on_DecimalFormatRadio_toggled(bool checked)
{
	if (!checked) return;

	ui.DecimalsSpin->setEnabled(true);

	if (refreshing_) return;

	numberFormat_.setType(NumberFormat::DecimalFormat);

	updatePreview();
}

void EditNumberFormatDialog::on_ScientificFormatRadio_toggled(bool checked)
{
	if (!checked) return;

	ui.DecimalsSpin->setEnabled(true);

	if (refreshing_) return;

	numberFormat_.setType(NumberFormat::ScientificFormat);

	updatePreview();
}

void EditNumberFormatDialog::on_DecimalsSpin_valueChanged(int value)
{
	if (refreshing_) return;

	numberFormat_.setNDecimals(value);

	updatePreview();
}

void EditNumberFormatDialog::on_UpperCaseExponentCheck_clicked(bool checked)
{
	if (refreshing_) return;

	numberFormat_.setUseUpperCaseExponent(checked);

	updatePreview();
}

void EditNumberFormatDialog::on_UseENotationCheck_clicked(bool checked)
{
	if (refreshing_) return;

	numberFormat_.setUseENotation(checked);

	updatePreview();
}

void EditNumberFormatDialog::on_PrecedeWithPlusCheck_clicked(bool checked)
{
	if (refreshing_) return;

	numberFormat_.setForcePrecedingPlus(checked);

	updatePreview();
}

// Update preview label
void EditNumberFormatDialog::updatePreview()
{
	ui.PreviewLabel->setText(numberFormat_.format(1.23456789));
}
