/*
	*** Operate - Background Subtraction Functions
	*** src/gui/uchroma/gui/operate_bgsub_funcs.cpp
	Copyright T. Youngs 2012-2015

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

#include "gui/uchroma/gui/operate_bgsub.h"
#include "gui/uchroma/uchromabase.h"
#include "gui/uchroma/classes/collection.h"
#include "gui/uchroma/expression/variable.h"
#include <QFileDialog>
#include <QMessageBox>

//.Static members
Collection* OperateBGSubDialog::targetCollection_ = NULL;
double OperateBGSubDialog::constantValue_ = 1.0;
double OperateBGSubDialog::xRangeMin_ = 0.0;
double OperateBGSubDialog::xRangeMax_ = 10.0;
double OperateBGSubDialog::zRangeMin_ = 0.0;
double OperateBGSubDialog::zRangeMax_ = 10.0;
int OperateBGSubDialog::subtractionMethod_ = 0;

// Constructor
OperateBGSubDialog::OperateBGSubDialog(UChromaBase& uChromaBase, QWidget* parent) : uChromaBase_(uChromaBase), QDialog(parent)
{
	ui.setupUi(this);

	refreshing_ = false;

	updateControls();
}

// Destructor
OperateBGSubDialog::~OperateBGSubDialog()
{
}

/*
 * Local Variables
 */

// Set constant y value to show
void OperateBGSubDialog::setConstantValue(double value)
{
	constantValue_ = value;
}

// Set x range to show
void OperateBGSubDialog::setXRange(double xMin, double xMax)
{
	xRangeMin_ = xMin;
	xRangeMax_ = xMax;
}

// Set z range to show
void OperateBGSubDialog::setZRange(double zMin, double zMax)
{
	zRangeMin_ = zMin;
	zRangeMax_ = zMax;
}

/*
 * Slots
 */

void OperateBGSubDialog::on_ConstantValueRadio_toggled(bool checked)
{
	if (checked)
	{
		toggleControls(0);
		subtractionMethod_ = 0;
	}
}

void OperateBGSubDialog::on_ConstantValueSpin_valueChanged(double value)
{
	constantValue_ = value;
}

void OperateBGSubDialog::on_ConstantValueSelectButton_clicked(bool checked)
{
	uChromaBase_.setInteractionMode(UChromaBase::OperateBGSubSelectYValueInteraction, 1);
	reject();
}

void OperateBGSubDialog::on_ConstantValueSetYMinimumButton_clicked(bool checked)
{
	ui.ConstantValueSpin->setValue(targetCollection_->transformMin().y);
}

void OperateBGSubDialog::on_ConstantValueSetYMaximumButton_clicked(bool checked)
{
	ui.ConstantValueSpin->setValue(targetCollection_->transformMax().y);
}

void OperateBGSubDialog::on_AverageOverXRadio_toggled(bool checked)
{
	if (checked)
	{
		toggleControls(1);
		subtractionMethod_= 1;
	}
}

void OperateBGSubDialog::on_XRangeMinSpin_valueChanged(double value)
{
	xRangeMin_ = value;
}

void OperateBGSubDialog::on_XRangeMaxSpin_valueChanged(double value)
{
	xRangeMax_ = value;
}

void OperateBGSubDialog::on_XRangeSelectButton_clicked(bool checked)
{
	uChromaBase_.setInteractionMode(UChromaBase::OperateBGSubSelectXRangeInteraction, 0);
	reject();
}

void OperateBGSubDialog::on_AverageOverZRadio_toggled(bool checked)
{
	if (checked)
	{
		toggleControls(2);
		subtractionMethod_ = 2;
	}
}

void OperateBGSubDialog::on_ZRangeMinSpin_valueChanged(double value)
{
	zRangeMin_ = value;
}

void OperateBGSubDialog::on_ZRangeMaxSpin_valueChanged(double value)
{
	zRangeMax_ = value;
}

void OperateBGSubDialog::on_ZRangeSelectButton_clicked(bool checked)
{
	uChromaBase_.setInteractionMode(UChromaBase::OperateBGSubSelectXRangeInteraction, 2);
	reject();
}

void OperateBGSubDialog::on_CancelButton_clicked(bool checked)
{
	reject();
}

void OperateBGSubDialog::on_ApplyButton_clicked(bool checked)
{
	if (ui.ConstantValueRadio->isChecked()) targetCollection_->addConstantValue(1, -constantValue_);
	else if (ui.AverageOverXRadio->isChecked())
	{
		for (DataSet* dataSet = targetCollection_->dataSets(); dataSet != NULL; dataSet = dataSet->next)
		{
			double avg = dataSet->averageY(xRangeMin_, xRangeMax_);
			Messenger::print("Average level (%e < x  %e) for dataset '%s' is %e\n", xRangeMin_, xRangeMax_, qPrintable(dataSet->name()), avg);
			dataSet->addConstantValue(1, -avg);
		}
	}
// 	else if (ui.AverageOverZRadio->isChecked()) result = setZFromSourceFiles();

	targetCollection_ = NULL;
	accept();
}

/*
 * Functions
 */

// Enable / disable controls
void OperateBGSubDialog::toggleControls(int index)
{
	ui.ConstantValueSpin->setEnabled(index == 0);
	ui.ConstantValueSetYMinimumButton->setEnabled(index == 0);
	ui.ConstantValueSetYMaximumButton->setEnabled(index == 0);
	ui.ConstantValueSelectButton->setEnabled(index == 0);
	ui.XRangeMinSpin->setEnabled(index == 1);
	ui.XRangeMaxSpin->setEnabled(index == 1);
	ui.XRangeSelectButton->setEnabled(index == 1);
	ui.ZRangeMinSpin->setEnabled(index == 2);
	ui.ZRangeMaxSpin->setEnabled(index == 2);
	ui.ZRangeSelectButton->setEnabled(index == 2);
}

/*
 * Update / Show
 */

// Show dialog, targetting specified collection
bool OperateBGSubDialog::updateAndExec(Collection* targetCollection)
{
	targetCollection_ = targetCollection;
	if (!Collection::objectValid(targetCollection_, "target collection in OperateBGSubDialog::updateAndExec(Collection*)")) return false;

	updateControls();

	return exec();
}

// Show dialog, targetting previously-set collection
bool OperateBGSubDialog::updateAndExec()
{
	if (!Collection::objectValid(targetCollection_, "target collection in OperateBGSubDialog::updateAndExec()")) return false;

	updateControls();

	return exec();
}

// Update controls
void OperateBGSubDialog::updateControls()
{
	refreshing_ = true;

	if (subtractionMethod_ == 0) ui.ConstantValueRadio->setChecked(true);
	else if (subtractionMethod_ == 1) ui.AverageOverXRadio->setChecked(true);
	else if (subtractionMethod_ == 2) ui.AverageOverZRadio->setChecked(true);
	ui.ConstantValueSpin->setValue(constantValue_);
	ui.XRangeMinSpin->setValue(xRangeMin_);
	ui.XRangeMaxSpin->setValue(xRangeMax_);
	ui.ZRangeMinSpin->setValue(zRangeMin_);
	ui.ZRangeMaxSpin->setValue(zRangeMax_);

	if (targetCollection_) setWindowTitle(QString("Background Subtraction (") + targetCollection_->name() + ")");

	refreshing_ = false;
}
