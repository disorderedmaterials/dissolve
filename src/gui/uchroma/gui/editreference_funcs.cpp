/*
	*** Reference Setup Dialog
	*** src/gui/uchroma/gui/referencedialog_funcs.cpp
	Copyright T. Youngs 2013-2018

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

#include "gui/uchroma/gui/editreference.h"
#include "gui/uchroma/uchromabase.h"
#include "gui/uchroma/kernels/fit.h"

// Constructor
EditReferenceDialog::EditReferenceDialog(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);
}

// Destructor
EditReferenceDialog::~EditReferenceDialog()
{
}

/*
 * Window Functions
 */

// Window close event
void EditReferenceDialog::closeEvent(QCloseEvent* event)
{
	reject();
}

void EditReferenceDialog::reject()
{
	setResult(QDialog::Rejected);
	hide();
}

void EditReferenceDialog::on_OKButton_clicked(bool checked)
{
	accept();
}

void EditReferenceDialog::on_CancelButton_clicked(bool checked)
{
	reject();
}

// Call dialog to edit specified reference
bool EditReferenceDialog::call(ReferenceVariable* target, FitKernel* parentKernel)
{
	// Check supplied var
	if (!parentKernel)
	{
		Messenger::print("Internal Error: NULL ReferenceVariable or FitKernel given to EditReferenceDialog::call().\n");
		return false;
	}
	if (!target->sourceCollection())
	{
		Messenger::print("Internal Error: No SourceCollection specified in ReferenceVariable.\n");
		return false;
	}

	// Take copy of supplied data
	referenceParent_ = parentKernel;
	originalPointer_ = target;
	reference_ = (*target);

	refreshing_ = true;
	
	// Update widgets
	// -- Names
	ui.NameEdit->setText(reference_.name());
	ui.SourceCollectionLabel->setText(reference_.sourceCollection()->name());
	// -- X range
	if (reference_.xIndex().type() == IndexData::NormalIndex) ui.XNormalRadio->setChecked(true);
	else if (reference_.xIndex().type() == IndexData::FixedIndex) ui.XFixedRadio->setChecked(true);
	else if (reference_.xIndex().type() == IndexData::RelativeIndex) ui.XRelativeRadio->setChecked(true);
	int nX = reference_.sourceCollection()->displayAbscissa().nItems();
	ui.XFixedSpin->setValue(reference_.xIndex().index()+1);
	ui.XFixedSpin->setRange(1, nX);
	ui.XRelativeSpin->setValue(reference_.xIndex().index());
	ui.XRelativeSpin->setRange(-nX, nX);
	// -- X range
	if (reference_.zIndex().type() == IndexData::NormalIndex) ui.ZNormalRadio->setChecked(true);
	else if (reference_.zIndex().type() == IndexData::FixedIndex) ui.ZFixedRadio->setChecked(true);
	else if (reference_.zIndex().type() == IndexData::RelativeIndex) ui.ZRelativeRadio->setChecked(true);
	int nZ = reference_.sourceCollection()->nDataSets();
	ui.ZDataSetCombo->clear();
	for (DataSet* dataSet = reference_.sourceCollection()->dataSets(); dataSet != NULL; dataSet = dataSet->next) ui.ZDataSetCombo->addItem(dataSet->name());
	ui.ZDataSetCombo->setCurrentIndex(reference_.zIndex().index());
	ui.ZRelativeSpin->setValue(reference_.zIndex().index());
	ui.ZRelativeSpin->setRange(-nZ, nZ);

	refreshing_ = false;

	// Update labels
	updateLabels();

	// Call the dialog
	if (exec()) return true;
	else return false;
}

// Return edited data
ReferenceVariable& EditReferenceDialog::reference()
{
	return reference_;
}

/*
 * Slots
 */

void EditReferenceDialog::on_NameEdit_textChanged(QString text)
{
	if (refreshing_) return;

	reference_.setName(qPrintable(text));

	// Must check name against other references and some standard variables to prevent duplicates...
	bool valid = true;
	ReferenceVariable* refVar = referenceParent_->reference(qPrintable(text));
	if (text == "x") valid = false;
	else if (text == "z") valid = false;
	else if (text.isEmpty()) valid = false;
	else if ((refVar != NULL) && (refVar != originalPointer_)) valid = false;

	if (valid)
	{
		ui.OKButton->setEnabled(true);
		ui.NameEdit->setPalette(ui.SourceCollectionLabel->palette());
	}
	else
	{
		ui.OKButton->setEnabled(false);
		QPalette redText;
		redText.setColor(QPalette::Text, Qt::red);
		ui.NameEdit->setPalette(redText);
	}
}

void EditReferenceDialog::on_XNormalRadio_clicked(bool checked)
{
	if (!checked) return;

	ui.XFixedSpin->setEnabled(false);
	ui.XFixedLabel->setEnabled(false);
	ui.XFixedSpinLabel->setEnabled(false);
	ui.XRelativeSpin->setEnabled(false);
	ui.XRelativeSpinLabel->setEnabled(false);

	if (refreshing_) return;

	reference_.xIndex().setType(IndexData::NormalIndex);
}

void EditReferenceDialog::on_XFixedRadio_clicked(bool checked)
{
	if (!checked) return;

	ui.XFixedSpin->setEnabled(true);
	ui.XFixedLabel->setEnabled(true);
	ui.XFixedSpinLabel->setEnabled(true);
	ui.XRelativeSpin->setEnabled(false);
	ui.XRelativeSpinLabel->setEnabled(false);

	if (refreshing_) return;

	reference_.xIndex().setType(IndexData::FixedIndex);
}

void EditReferenceDialog::on_XRelativeRadio_clicked(bool checked)
{
	if (!checked) return;

	ui.XFixedSpin->setEnabled(false);
	ui.XFixedLabel->setEnabled(false);
	ui.XFixedSpinLabel->setEnabled(false);
	ui.XRelativeSpin->setEnabled(true);
	ui.XRelativeSpinLabel->setEnabled(true);

	if (refreshing_) return;

	reference_.xIndex().setType(IndexData::RelativeIndex);
}

void EditReferenceDialog::on_XFixedSpin_valueChanged(int value)
{
	if (refreshing_) return;

	reference_.xIndex().setIndex(value-1);

	updateLabels();
}

void EditReferenceDialog::on_XRelativeSpin_valueChanged(int value)
{
	if (refreshing_) return;

	reference_.xIndex().setOffset(value-1);

	updateLabels();
}

void EditReferenceDialog::on_ZNormalRadio_clicked(bool checked)
{
	if (!checked) return;

	ui.ZDataSetCombo->setEnabled(false);
	ui.ZFixedLabel->setEnabled(false);
	ui.ZDataSetComboLabel->setEnabled(false);
	ui.ZRelativeSpin->setEnabled(false);
	ui.ZRelativeSpinLabel->setEnabled(false);

	if (refreshing_) return;

	reference_.zIndex().setType(IndexData::NormalIndex);
}

void EditReferenceDialog::on_ZFixedRadio_clicked(bool checked)
{
	if (!checked) return;

	ui.ZDataSetCombo->setEnabled(true);
	ui.ZFixedLabel->setEnabled(true);
	ui.ZDataSetComboLabel->setEnabled(true);
	ui.ZRelativeSpin->setEnabled(false);
	ui.ZRelativeSpinLabel->setEnabled(false);

	if (refreshing_) return;

	reference_.zIndex().setType(IndexData::FixedIndex);
}

void EditReferenceDialog::on_ZRelativeRadio_clicked(bool checked)
{
	if (!checked) return;

	ui.ZDataSetCombo->setEnabled(false);
	ui.ZFixedLabel->setEnabled(false);
	ui.ZDataSetComboLabel->setEnabled(false);
	ui.ZRelativeSpin->setEnabled(true);
	ui.ZRelativeSpinLabel->setEnabled(true);

	if (refreshing_) return;

	reference_.zIndex().setType(IndexData::RelativeIndex);
}

void EditReferenceDialog::on_ZDataSetCombo_currentIndexChanged(int index)
{
	if (refreshing_) return;

	reference_.zIndex().setIndex(index);
	reference_.setZDataSetName(qPrintable(ui.ZDataSetCombo->currentText()));

	updateLabels();
}

void EditReferenceDialog::on_ZRelativeSpin_valueChanged(int value)
{
	if (refreshing_) return;

	reference_.zIndex().setOffset(value-1);

	updateLabels();
}

// Update labels
void EditReferenceDialog::updateLabels()
{
	if (refreshing_) return;

	const Array<double>& abscissa = reference_.sourceCollection()->displayAbscissa();
	ui.XFixedLabel->setText("(X = " + QString::number(abscissa.constAt(ui.XFixedSpin->value())) + ")");

	// Z Source
	DataSet* dataSet = reference_.sourceCollection()->nDataSets() == 0 ? NULL : reference_.sourceCollection()->dataSet(ui.ZDataSetCombo->currentIndex());
	ui.ZFixedLabel->setText("(Z = " + (dataSet ? QString::number(dataSet->z()) + ")" : "?)"));
}
